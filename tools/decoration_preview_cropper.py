"""Interactively crop a decoration preview and export a transparent WebP."""

from __future__ import annotations

import math
import re
import sys
from collections.abc import Callable, Sequence
from pathlib import Path

from PySide6.QtCore import QPoint, QPointF, QRect, Qt, Signal
from PySide6.QtGui import (
    QColor,
    QDragEnterEvent,
    QDropEvent,
    QFont,
    QImage,
    QImageReader,
    QImageWriter,
    QPainter,
    QPainterPath,
    QPen,
    QPixmap,
    QPolygonF,
)
from PySide6.QtWidgets import (
    QApplication,
    QFileDialog,
    QFrame,
    QGraphicsEllipseItem,
    QGraphicsItem,
    QGraphicsPathItem,
    QGraphicsPixmapItem,
    QGraphicsPolygonItem,
    QGraphicsScene,
    QGraphicsSimpleTextItem,
    QGraphicsView,
    QHBoxLayout,
    QLabel,
    QMainWindow,
    QMessageBox,
    QPushButton,
    QVBoxLayout,
    QWidget,
)


WINDOW_TITLE = "Decoration Preview Cropper"
WINDOW_MINIMUM_WIDTH = 1080
WINDOW_MINIMUM_HEIGHT = 720
CONTROL_PANEL_WIDTH = 300
HANDLE_RADIUS = 9.0
HANDLE_LABEL_OFFSET_X = -4.0
HANDLE_LABEL_OFFSET_Y = -8.0
ZOOM_STEP = 1.15
MINIMUM_ZOOM = 0.08
MAXIMUM_ZOOM = 12.0
WEBP_QUALITY = 100
THUMBNAIL_WIDTH = 300
THUMBNAIL_HEIGHT = 200
SUPPORTED_IMAGE_SUFFIXES = {".png", ".bmp"}

COLOR_WINDOW = "#0d1114"
COLOR_PANEL = "#171c20"
COLOR_PANEL_BORDER = "#2b3339"
COLOR_CANVAS = "#07090b"
COLOR_TEXT = "#edf1f4"
COLOR_MUTED = "#8f9ba4"
COLOR_ACCENT = "#ff5454"
COLOR_ACCENT_HOVER = "#ff6b6b"
COLOR_BUTTON = "#252d33"
COLOR_BUTTON_HOVER = "#313b43"

APPLICATION_STYLE = f"""
QMainWindow {{
    background: {COLOR_WINDOW};
}}
QWidget {{
    color: {COLOR_TEXT};
    font-family: "Microsoft YaHei UI";
    font-size: 13px;
}}
QFrame#controlPanel {{
    background: {COLOR_PANEL};
    border-left: 1px solid {COLOR_PANEL_BORDER};
}}
QLabel#titleLabel {{
    font-size: 20px;
    font-weight: 700;
}}
QLabel#sectionLabel {{
    color: {COLOR_MUTED};
    font-size: 11px;
    font-weight: 700;
    letter-spacing: 1px;
}}
QLabel#statusLabel {{
    color: {COLOR_MUTED};
}}
QPushButton {{
    min-height: 34px;
    padding: 0 12px;
    border: 1px solid {COLOR_PANEL_BORDER};
    border-radius: 5px;
    background: {COLOR_BUTTON};
}}
QPushButton:hover {{
    background: {COLOR_BUTTON_HOVER};
}}
QPushButton:disabled {{
    color: #59636b;
    background: #1c2226;
}}
QPushButton#primaryButton {{
    color: #ffffff;
    border-color: {COLOR_ACCENT};
    background: {COLOR_ACCENT};
    font-weight: 700;
}}
QPushButton#primaryButton:hover {{
    background: {COLOR_ACCENT_HOVER};
}}
"""


def calculate_crop_rect(image: QImage, points: Sequence[QPointF]) -> QRect:
    """Return the inclusive image-pixel bounds of four crop points."""
    if len(points) != 4:
        raise ValueError("Exactly four crop points are required.")

    minimum_x = points[0].x()
    minimum_y = points[0].y()
    maximum_x = points[0].x()
    maximum_y = points[0].y()

    point_index = 1
    while point_index < len(points):
        point = points[point_index]
        minimum_x = min(minimum_x, point.x())
        minimum_y = min(minimum_y, point.y())
        maximum_x = max(maximum_x, point.x())
        maximum_y = max(maximum_y, point.y())
        point_index += 1

    left = max(0, math.floor(minimum_x))
    top = max(0, math.floor(minimum_y))
    right = min(image.width() - 1, math.ceil(maximum_x))
    bottom = min(image.height() - 1, math.ceil(maximum_y))

    if right < left or bottom < top:
        raise ValueError("Crop points do not intersect the source image.")

    return QRect(left, top, right - left + 1, bottom - top + 1)


def remove_exact_black(image: QImage) -> QImage:
    """Return a copy whose exact #000000 pixels are fully transparent."""
    transparent_image = image.convertToFormat(QImage.Format_RGBA8888)
    pixel_data = transparent_image.bits()
    bytes_per_line = transparent_image.bytesPerLine()

    y = 0
    while y < transparent_image.height():
        row_offset = y * bytes_per_line
        x = 0
        while x < transparent_image.width():
            pixel_offset = row_offset + x * 4
            red = pixel_data[pixel_offset]
            green = pixel_data[pixel_offset + 1]
            blue = pixel_data[pixel_offset + 2]

            if red == 0 and green == 0 and blue == 0:
                pixel_data[pixel_offset + 3] = 0

            x += 1
        y += 1

    return transparent_image


def build_cropped_image(source_image: QImage, points: Sequence[QPointF]) -> QImage:
    """Mask a four-point region, crop its bounds, and remove pure black."""
    crop_rect = calculate_crop_rect(source_image, points)
    shifted_polygon = QPolygonF()

    for point in points:
        shifted_x = point.x() - crop_rect.left()
        shifted_y = point.y() - crop_rect.top()
        shifted_polygon.append(QPointF(shifted_x, shifted_y))

    cropped_image = QImage(crop_rect.size(), QImage.Format_RGBA8888)
    cropped_image.fill(Qt.transparent)

    painter = QPainter(cropped_image)
    painter.setRenderHint(QPainter.Antialiasing, True)
    clip_path = QPainterPath()
    clip_path.addPolygon(shifted_polygon)
    clip_path.closeSubpath()
    painter.setClipPath(clip_path)
    painter.drawImage(QPoint(0, 0), source_image, crop_rect)
    painter.end()

    return remove_exact_black(cropped_image)


def build_default_output_path(source_path: Path) -> Path:
    """Build a friendly WebP path and remove a trailing tile index."""
    output_stem = re.sub(r"_-?\d+_-?\d+$", "", source_path.stem)
    return source_path.with_name(f"{output_stem}.webp")


def build_thumbnail_output_path(output_path: Path) -> Path:
    """Return the companion thumbnail path for one exported preview."""
    return output_path.with_name(f"{output_path.stem}_thumbnail.webp")


def build_thumbnail(image: QImage) -> QImage:
    """Fit an image inside a centered, opaque black 300x200 canvas."""
    thumbnail = QImage(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT, QImage.Format_RGB32)
    thumbnail.fill(QColor(0, 0, 0))

    scaled_image = image.scaled(
        THUMBNAIL_WIDTH,
        THUMBNAIL_HEIGHT,
        Qt.KeepAspectRatio,
        Qt.SmoothTransformation,
    )
    offset_x = (THUMBNAIL_WIDTH - scaled_image.width()) // 2
    offset_y = (THUMBNAIL_HEIGHT - scaled_image.height()) // 2

    painter = QPainter(thumbnail)
    painter.drawImage(QPoint(offset_x, offset_y), scaled_image)
    painter.end()
    return thumbnail


def save_webp(image: QImage, output_path: Path) -> None:
    """Write an alpha-preserving, maximum-quality WebP image."""
    writer = QImageWriter(str(output_path), b"webp")
    writer.setQuality(WEBP_QUALITY)
    writer.setOptimizedWrite(True)

    if not writer.write(image):
        raise RuntimeError(writer.errorString())


class CropHandle(QGraphicsEllipseItem):
    """Movable numbered handle for one crop polygon corner."""

    def __init__(self, number: int, moved_callback: Callable[[], None]) -> None:
        super().__init__(
            -HANDLE_RADIUS,
            -HANDLE_RADIUS,
            HANDLE_RADIUS * 2.0,
            HANDLE_RADIUS * 2.0,
        )
        self.moved_callback = moved_callback
        self.setBrush(QColor(COLOR_ACCENT))

        pen = QPen(QColor("#ffffff"))
        pen.setWidth(2)
        pen.setCosmetic(True)
        self.setPen(pen)

        self.setFlag(QGraphicsItem.ItemIsMovable, True)
        self.setFlag(QGraphicsItem.ItemSendsGeometryChanges, True)
        self.setFlag(QGraphicsItem.ItemIgnoresTransformations, True)
        self.setZValue(30.0)

        label = QGraphicsSimpleTextItem(str(number), self)
        label.setFont(QFont("Arial", 9, QFont.Bold))
        label.setBrush(QColor("#ffffff"))
        label.setPos(HANDLE_LABEL_OFFSET_X, HANDLE_LABEL_OFFSET_Y)
        label.setAcceptedMouseButtons(Qt.NoButton)

    def itemChange(self, change, value):
        """Clamp drag movement to the image and refresh the polygon."""
        if change == QGraphicsItem.ItemPositionChange and self.scene() is not None:
            scene_rect = self.scene().sceneRect()
            requested_position = value
            clamped_x = min(max(requested_position.x(), scene_rect.left()), scene_rect.right())
            clamped_y = min(max(requested_position.y(), scene_rect.top()), scene_rect.bottom())
            return QPointF(clamped_x, clamped_y)

        if change == QGraphicsItem.ItemPositionHasChanged:
            self.moved_callback()

        return super().itemChange(change, value)


class CropCanvas(QGraphicsView):
    """Image canvas that collects and adjusts four crop points."""

    points_changed = Signal(int)

    def __init__(self) -> None:
        super().__init__()
        self.source_image = QImage()
        self.pixmap_item: QGraphicsPixmapItem | None = None
        self.polygon_item = QGraphicsPolygonItem()
        self.outside_mask_item = QGraphicsPathItem()
        self.handles: list[CropHandle] = []

        scene = self.scene()
        if scene is None:
            scene = QGraphicsScene(self)
            self.setScene(scene)

        polygon_pen = QPen(QColor(COLOR_ACCENT))
        polygon_pen.setWidth(3)
        polygon_pen.setCosmetic(True)
        self.polygon_item.setPen(polygon_pen)
        self.polygon_item.setBrush(QColor(255, 84, 84, 24))
        self.polygon_item.setZValue(20.0)
        scene.addItem(self.polygon_item)

        self.outside_mask_item.setPen(Qt.NoPen)
        self.outside_mask_item.setBrush(QColor(0, 0, 0, 125))
        self.outside_mask_item.setZValue(10.0)
        scene.addItem(self.outside_mask_item)

        self.setBackgroundBrush(QColor(COLOR_CANVAS))
        self.setRenderHint(QPainter.Antialiasing, True)
        self.setTransformationAnchor(QGraphicsView.AnchorUnderMouse)
        self.setResizeAnchor(QGraphicsView.AnchorViewCenter)

    def load_image(self, image: QImage) -> None:
        """Display a new source image and clear the current polygon."""
        scene = self.scene()
        if scene is None:
            return

        if self.pixmap_item is not None:
            scene.removeItem(self.pixmap_item)

        self.source_image = image
        self.pixmap_item = QGraphicsPixmapItem(QPixmap.fromImage(image))
        self.pixmap_item.setZValue(0.0)
        scene.addItem(self.pixmap_item)
        scene.setSceneRect(0.0, 0.0, float(image.width()), float(image.height()))

        self.reset_points()
        self.fit_image()

    def crop_points(self) -> list[QPointF]:
        """Return current crop points in source-image coordinates."""
        points = []
        for handle in self.handles:
            points.append(QPointF(handle.pos()))
        return points

    def undo_point(self) -> None:
        """Remove the most recently added crop point."""
        if not self.handles:
            return

        handle = self.handles.pop()
        scene = self.scene()
        if scene is not None:
            scene.removeItem(handle)
        self._refresh_overlay()

    def reset_points(self) -> None:
        """Remove all crop points."""
        scene = self.scene()
        if scene is not None:
            for handle in self.handles:
                scene.removeItem(handle)

        self.handles.clear()
        self._refresh_overlay()

    def fit_image(self) -> None:
        """Fit the complete source image into the canvas."""
        if self.source_image.isNull():
            return

        self.resetTransform()
        self.fitInView(self.sceneRect(), Qt.KeepAspectRatio)

    def mousePressEvent(self, event) -> None:
        """Add a point or let an existing handle receive the mouse event."""
        if event.button() != Qt.LeftButton:
            super().mousePressEvent(event)
            return

        clicked_item = self.itemAt(event.position().toPoint())
        current_item = clicked_item
        while current_item is not None:
            if isinstance(current_item, CropHandle):
                super().mousePressEvent(event)
                return
            current_item = current_item.parentItem()

        if self.source_image.isNull() or len(self.handles) >= 4:
            super().mousePressEvent(event)
            return

        scene_point = self.mapToScene(event.position().toPoint())
        if not self.sceneRect().contains(scene_point):
            return

        self._add_handle(scene_point)
        event.accept()

    def wheelEvent(self, event) -> None:
        """Zoom around the cursor for precise corner placement."""
        current_zoom = self.transform().m11()
        if event.angleDelta().y() > 0:
            requested_zoom = current_zoom * ZOOM_STEP
            if requested_zoom <= MAXIMUM_ZOOM:
                self.scale(ZOOM_STEP, ZOOM_STEP)
        else:
            requested_zoom = current_zoom / ZOOM_STEP
            if requested_zoom >= MINIMUM_ZOOM:
                self.scale(1.0 / ZOOM_STEP, 1.0 / ZOOM_STEP)

        event.accept()

    def _add_handle(self, position: QPointF) -> None:
        """Create one numbered point at an image coordinate."""
        handle = CropHandle(len(self.handles) + 1, self._refresh_overlay)
        self.handles.append(handle)

        scene = self.scene()
        if scene is not None:
            scene.addItem(handle)
        handle.setPos(position)
        self._refresh_overlay()

    def _refresh_overlay(self) -> None:
        """Redraw the crop boundary and outside dimming mask."""
        polygon = QPolygonF()
        for handle in self.handles:
            polygon.append(handle.pos())

        self.polygon_item.setPolygon(polygon)

        mask_path = QPainterPath()
        if len(self.handles) == 4:
            mask_path.setFillRule(Qt.OddEvenFill)
            mask_path.addRect(self.sceneRect())
            mask_path.addPolygon(polygon)
            mask_path.closeSubpath()
        self.outside_mask_item.setPath(mask_path)
        self.points_changed.emit(len(self.handles))


class DecorationPreviewCropper(QMainWindow):
    """Small desktop utility for preparing one decoration preview image."""

    def __init__(self, initial_path: Path | None = None) -> None:
        super().__init__()
        self.source_path: Path | None = None
        self.canvas = CropCanvas()
        self.canvas.setAcceptDrops(False)
        self.canvas.viewport().setAcceptDrops(False)
        self.canvas.points_changed.connect(self._update_point_status)

        self.file_label = QLabel("尚未打开图片")
        self.file_label.setWordWrap(True)
        self.file_label.setObjectName("statusLabel")

        self.image_size_label = QLabel("—")
        self.image_size_label.setObjectName("statusLabel")

        self.point_status_label = QLabel("0 / 4")
        self.point_status_label.setObjectName("statusLabel")

        self.undo_button = QPushButton("撤销最后一点")
        self.undo_button.clicked.connect(self.canvas.undo_point)
        self.undo_button.setEnabled(False)

        self.reset_button = QPushButton("重新标记")
        self.reset_button.clicked.connect(self.canvas.reset_points)
        self.reset_button.setEnabled(False)

        self.export_button = QPushButton("导出原图 + 缩略图")
        self.export_button.setObjectName("primaryButton")
        self.export_button.clicked.connect(self._export_webp)
        self.export_button.setEnabled(False)

        self._build_window()
        self.setAcceptDrops(True)

        if initial_path is not None:
            self._load_path(initial_path)

    def _build_window(self) -> None:
        """Build the focused canvas-and-controls layout."""
        self.setWindowTitle(WINDOW_TITLE)
        self.setMinimumSize(WINDOW_MINIMUM_WIDTH, WINDOW_MINIMUM_HEIGHT)

        root = QWidget()
        root_layout = QHBoxLayout(root)
        root_layout.setContentsMargins(0, 0, 0, 0)
        root_layout.setSpacing(0)
        root_layout.addWidget(self.canvas, 1)
        root_layout.addWidget(self._build_control_panel())
        self.setCentralWidget(root)

    def _build_control_panel(self) -> QFrame:
        """Build the compact instruction and action panel."""
        panel = QFrame()
        panel.setObjectName("controlPanel")
        panel.setFixedWidth(CONTROL_PANEL_WIDTH)
        layout = QVBoxLayout(panel)
        layout.setContentsMargins(20, 22, 20, 20)
        layout.setSpacing(12)

        title = QLabel("斜裁预览图")
        title.setObjectName("titleLabel")
        layout.addWidget(title)

        description = QLabel(
            "直接拖入 PNG/BMP，或点击按钮打开。沿目标区域顺时针或逆时针点击四个角，完成后可拖动红色编号点微调。"
        )
        description.setWordWrap(True)
        layout.addWidget(description)

        open_button = QPushButton("打开 PNG / BMP")
        open_button.clicked.connect(self._open_image)
        layout.addWidget(open_button)

        section = QLabel("SOURCE")
        section.setObjectName("sectionLabel")
        layout.addWidget(section)
        layout.addWidget(self.file_label)
        layout.addWidget(self.image_size_label)

        point_section = QLabel("CROP POINTS")
        point_section.setObjectName("sectionLabel")
        layout.addWidget(point_section)
        layout.addWidget(self.point_status_label)
        layout.addWidget(self.undo_button)
        layout.addWidget(self.reset_button)

        fit_button = QPushButton("适应窗口")
        fit_button.clicked.connect(self.canvas.fit_image)
        layout.addWidget(fit_button)

        export_note = QLabel(
            "原始裁剪图会抠除精确 #000000；300×200 等比例缩略图保留不透明纯黑背景。"
        )
        export_note.setWordWrap(True)
        export_note.setObjectName("statusLabel")
        layout.addWidget(export_note)

        layout.addStretch(1)
        layout.addWidget(self.export_button)
        return panel

    def dragEnterEvent(self, event: QDragEnterEvent) -> None:
        """Accept the first local PNG or BMP dragged over the window."""
        image_path = self._find_dropped_image(event.mimeData().urls())
        if image_path is None:
            event.ignore()
            return

        event.acceptProposedAction()

    def dropEvent(self, event: QDropEvent) -> None:
        """Load a supported image dropped anywhere on the tool window."""
        image_path = self._find_dropped_image(event.mimeData().urls())
        if image_path is None:
            event.ignore()
            return

        self._load_path(image_path)
        event.acceptProposedAction()

    def _open_image(self) -> None:
        """Ask the user for one PNG or BMP source image."""
        selected_path, _ = QFileDialog.getOpenFileName(
            self,
            "打开装饰预览原图",
            "",
            "Image files (*.png *.bmp)",
        )
        if not selected_path:
            return

        self._load_path(Path(selected_path))

    def _load_path(self, image_path: Path) -> None:
        """Read and display one supported source image."""
        reader = QImageReader(str(image_path))
        reader.setAutoTransform(True)
        image = reader.read()

        if image.isNull():
            QMessageBox.critical(
                self,
                "无法打开图片",
                reader.errorString(),
            )
            return

        self.source_path = image_path.resolve()
        self.canvas.load_image(image)
        self.file_label.setText(self.source_path.name)
        self.file_label.setToolTip(str(self.source_path))
        self.image_size_label.setText(f"{image.width()} × {image.height()} px")

    def _find_dropped_image(self, urls) -> Path | None:
        """Return the first supported local image from dropped URLs."""
        for url in urls:
            if not url.isLocalFile():
                continue

            image_path = Path(url.toLocalFile())
            if image_path.suffix.lower() in SUPPORTED_IMAGE_SUFFIXES:
                return image_path

        return None

    def _update_point_status(self, point_count: int) -> None:
        """Keep action availability aligned with the current polygon."""
        self.point_status_label.setText(f"{point_count} / 4")
        self.undo_button.setEnabled(point_count > 0)
        self.reset_button.setEnabled(point_count > 0)
        self.export_button.setEnabled(point_count == 4)

    def _export_webp(self) -> None:
        """Build and save the selected transparent WebP image."""
        if self.source_path is None:
            return

        points = self.canvas.crop_points()
        if len(points) != 4:
            return

        default_path = build_default_output_path(self.source_path)
        selected_path, _ = QFileDialog.getSaveFileName(
            self,
            "导出装饰预览",
            str(default_path),
            "WebP image (*.webp)",
        )
        if not selected_path:
            return

        output_path = Path(selected_path)
        if output_path.suffix.lower() != ".webp":
            output_path = output_path.with_suffix(".webp")

        cropped_image = build_cropped_image(self.canvas.source_image, points)
        thumbnail_image = build_thumbnail(cropped_image)
        thumbnail_path = build_thumbnail_output_path(output_path)

        try:
            save_webp(cropped_image, output_path)
            save_webp(thumbnail_image, thumbnail_path)
        except RuntimeError as error:
            QMessageBox.critical(self, "导出失败", str(error))
            return

        QMessageBox.information(
            self,
            "导出完成",
            (
                f"原始裁剪图：{output_path}\n"
                f"尺寸：{cropped_image.width()} × {cropped_image.height()} px\n\n"
                f"缩略图：{thumbnail_path}\n"
                f"尺寸：{THUMBNAIL_WIDTH} × {THUMBNAIL_HEIGHT} px"
            ),
        )


def main(arguments: Sequence[str] | None = None) -> int:
    """Start the interactive cropper."""
    if arguments is None:
        arguments = sys.argv

    app = QApplication(list(arguments))
    app.setStyleSheet(APPLICATION_STYLE)

    initial_path = None
    if len(arguments) > 1:
        initial_path = Path(arguments[1])

    window = DecorationPreviewCropper(initial_path)
    window.show()
    return app.exec()


if __name__ == "__main__":
    raise SystemExit(main())
