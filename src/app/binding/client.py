"""
Thin client for the Auto Mapper C++ DLL.
"""
import ctypes
from pathlib import Path

from app.binding.structures import (
    CDoor,
    CDrawablePart,
    CIncubatorArray,
    CSegment,
    CStandardDoorZConfig,
    CWallProfile,
)
from app.config import DLL_PATH
from app.logger import logger
from app.project.data import ProjectData


REQUIRED_API_VERSION = 2


class AutoMapperLibClient:
    """
    Isolated ctypes wrapper around libauto_mapper.dll.
    """

    def __init__(self, dll_path: Path = DLL_PATH) -> None:
        self.dll_path = dll_path
        self.lib = None

    def load(self) -> bool:
        """
        Load the DLL if it exists.
        """
        if self.lib is not None:
            return True

        if not self.dll_path.exists():
            logger.warning(f"DLL not found: {self.dll_path}")
            return False

        self.lib = ctypes.CDLL(str(self.dll_path))
        self._validate_api_version()
        self._configure_functions()
        logger.info(f"Loaded DLL: {self.dll_path}")
        return True

    def _configure_functions(self) -> None:
        """
        Configure ctypes function signatures.
        """
        if self.lib is None:
            return

        self.lib.generate_map_from_segments.argtypes = [
            ctypes.c_char_p,
            ctypes.POINTER(CSegment),
            ctypes.c_int,
            ctypes.POINTER(CDoor),
            ctypes.c_int,
            ctypes.POINTER(CIncubatorArray),
            ctypes.c_int,
            ctypes.c_float,
            ctypes.c_float,
            ctypes.c_bool,
            ctypes.c_bool,
            ctypes.c_bool,
        ]
        self.lib.generate_map_from_segments.restype = ctypes.c_bool

        self.lib.get_standard_door_z_config.argtypes = [
            ctypes.c_int,
            ctypes.POINTER(CStandardDoorZConfig),
        ]
        self.lib.get_standard_door_z_config.restype = ctypes.c_bool

        self.lib.get_standard_door_jam_z_offset.argtypes = [
            ctypes.c_int,
            ctypes.POINTER(ctypes.c_float),
        ]
        self.lib.get_standard_door_jam_z_offset.restype = ctypes.c_bool

        self.lib.get_standard_door_size_count.argtypes = []
        self.lib.get_standard_door_size_count.restype = ctypes.c_int

        self.lib.get_standard_door_size_at.argtypes = [
            ctypes.c_int,
            ctypes.POINTER(ctypes.c_int),
        ]
        self.lib.get_standard_door_size_at.restype = ctypes.c_bool

        self.lib.get_wall_profile_count.argtypes = []
        self.lib.get_wall_profile_count.restype = ctypes.c_int

        self.lib.get_wall_profile_type_at.argtypes = [
            ctypes.c_int,
            ctypes.POINTER(ctypes.c_int),
        ]
        self.lib.get_wall_profile_type_at.restype = ctypes.c_bool

        self.lib.get_wall_profile.argtypes = [
            ctypes.c_int,
            ctypes.POINTER(CWallProfile),
        ]
        self.lib.get_wall_profile.restype = ctypes.c_bool

        self.lib.get_wall_drawable_part_count.argtypes = [
            ctypes.c_int,
        ]
        self.lib.get_wall_drawable_part_count.restype = ctypes.c_int

        self.lib.get_wall_drawable_part_at.argtypes = [
            ctypes.c_int,
            ctypes.c_int,
            ctypes.POINTER(CDrawablePart),
        ]
        self.lib.get_wall_drawable_part_at.restype = ctypes.c_bool

    def _validate_api_version(self) -> None:
        """
        Ensure the loaded DLL matches this Python binding.
        """
        if self.lib is None:
            return

        try:
            version_function = self.lib.get_auto_mapper_api_version
        except AttributeError as error:
            raise RuntimeError(
                f"Loaded DLL is too old and has no API version: {self.dll_path}"
            ) from error

        version_function.argtypes = []
        version_function.restype = ctypes.c_int
        actual_version = version_function()

        if actual_version != REQUIRED_API_VERSION:
            raise RuntimeError(
                f"DLL API version mismatch: expected {REQUIRED_API_VERSION}, "
                f"got {actual_version}. DLL path: {self.dll_path}"
            )

    def load_standard_door_z_config(self, sizes: list = None) -> dict:
        """
        Load standard door z-offset configs from the DLL.
        """
        if not self.load():
            return {}

        configs = {}
        if sizes is None:
            sizes = self.load_standard_door_sizes()

        for size in sizes:
            config = CStandardDoorZConfig()
            success = self.lib.get_standard_door_z_config(size, ctypes.byref(config))
            if success:
                configs[size] = config

        logger.info(f"Loaded standard door z config: {sorted(configs.keys())}")
        return configs

    def load_standard_door_sizes(self) -> list:
        """
        Load supported standard door sizes from the DLL.
        """
        if not self.load():
            return []

        sizes = []
        count = self.lib.get_standard_door_size_count()

        index = 0
        while index < count:
            size_value = ctypes.c_int()
            success = self.lib.get_standard_door_size_at(index, ctypes.byref(size_value))
            if success:
                sizes.append(size_value.value)

            index += 1

        sizes.sort()
        logger.info(f"Loaded standard door sizes: {sizes}")
        return sizes

    def load_wall_profiles(self) -> dict:
        """
        Load all wall profiles exposed by the DLL.
        """
        if not self.load():
            return {}

        profiles = {}
        count = self.lib.get_wall_profile_count()

        index = 0
        while index < count:
            wall_type_value = ctypes.c_int()
            type_success = self.lib.get_wall_profile_type_at(index, ctypes.byref(wall_type_value))
            if type_success:
                c_profile = CWallProfile()
                profile_success = self.lib.get_wall_profile(wall_type_value.value, ctypes.byref(c_profile))
                if profile_success:
                    profiles[wall_type_value.value] = self._convert_wall_profile(c_profile)

            index += 1

        logger.info(f"Loaded wall profiles from DLL: {sorted(profiles.keys())}")
        return profiles

    def get_standard_door_jam_z_offset(self, size: int) -> float:
        """
        Get a jammed door z-offset from the DLL.
        """
        if not self.load():
            raise FileNotFoundError(f"DLL not found: {self.dll_path}")

        z_offset = ctypes.c_float()
        success = self.lib.get_standard_door_jam_z_offset(size, ctypes.byref(z_offset))
        if not success:
            raise RuntimeError(f"Failed to load jammed door z offset for size {size}.")

        return z_offset.value

    def generate_map(
        self,
        output_path: Path,
        project_data: ProjectData,
        generate_floor: bool = True,
        generate_ceiling: bool = True,
        random_direction: bool = False,
    ) -> bool:
        """
        Generate a .map file through the C++ DLL.
        """
        if not self.load():
            raise FileNotFoundError(f"DLL not found: {self.dll_path}")

        segment_array = self._build_segment_array(project_data.segments)
        door_array = self._build_door_array(project_data.doors, project_data.is_door_open)
        incubator_array = self._build_incubator_array(project_data.decorations)
        output_path_bytes = str(output_path).encode("utf-8")

        success = self.lib.generate_map_from_segments(
            output_path_bytes,
            segment_array,
            len(project_data.segments),
            door_array,
            len(project_data.doors),
            incubator_array,
            len(project_data.decorations),
            float(project_data.map_size_x),
            float(project_data.map_size_y),
            generate_floor,
            generate_ceiling,
            random_direction,
        )
        return bool(success)

    def _build_segment_array(self, segments: list):
        """
        Convert Python segment tuples into a C array.
        """
        SegmentArray = CSegment * len(segments)
        segment_array = SegmentArray()

        index = 0
        for segment in segments:
            start_point = segment[0]
            end_point = segment[1]
            wall_type = segment[2]

            segment_array[index].x1 = int(start_point[0])
            segment_array[index].y1 = int(start_point[1])
            segment_array[index].x2 = int(end_point[0])
            segment_array[index].y2 = int(end_point[1])
            segment_array[index].wall_type = int(wall_type)
            index += 1

        return segment_array

    def _build_door_array(self, doors: list, is_door_open: bool = False):
        """
        Convert Python door tuples into a C array, applying global is_door_open override
        only to Standard Active Doors (excluding dead doors and lab laser doors).
        """
        DoorArray = CDoor * len(doors)
        door_array = DoorArray()

        WALL_TYPE_STANDARD = 0
        WALL_TYPE_STANDARD_DARK = 2
        LIGHT_STATE_GREEN = 0
        LIGHT_STATE_RED = 1
        LIGHT_STATE_BROKEN = 2
        DOOR_STATE_OPEN = 1
        DOOR_STATE_CLOSED = 0

        index = 0
        for door in doors:
            pos_x = int(door[0])
            pos_y = int(door[1])
            wall_type = int(door[2])
            direction_type = int(door[3])
            size = int(door[4])
            door_state = int(door[5])
            light_state = int(door[6])
            z_offset = float(door[7])

            # Apply global state only to standard-family active doors.
            is_active_door = (light_state != LIGHT_STATE_BROKEN)
            is_standard_family = wall_type == WALL_TYPE_STANDARD or wall_type == WALL_TYPE_STANDARD_DARK
            if is_standard_family and is_active_door:
                if is_door_open:
                    light_state = LIGHT_STATE_GREEN
                    door_state = DOOR_STATE_OPEN
                else:
                    light_state = LIGHT_STATE_RED
                    door_state = DOOR_STATE_CLOSED

            door_array[index].x = pos_x
            door_array[index].y = pos_y
            door_array[index].wall_type = wall_type
            door_array[index].direction_type = direction_type
            door_array[index].size = size
            door_array[index].door_state = door_state
            door_array[index].light_state = light_state
            door_array[index].z_offset = z_offset
            index += 1

        return door_array

    def _build_incubator_array(self, decorations: list):
        """
        Convert incubator decorations into a C array.
        """
        IncubatorArray = CIncubatorArray * len(decorations)
        incubator_array = IncubatorArray()

        index = 0
        for decoration in decorations:
            incubator_array[index].start_x = float(decoration.start_x)
            incubator_array[index].start_y = float(decoration.start_y)
            incubator_array[index].row_length = float(decoration.row_length)
            incubator_array[index].column_length = float(decoration.column_length)
            incubator_array[index].item_spacing_scale = float(decoration.item_spacing_scale)
            incubator_array[index].row_spacing_scale = float(decoration.row_spacing_scale)
            index += 1

        return incubator_array

    def _convert_wall_profile(self, c_profile: CWallProfile) -> dict:
        """
        Convert a C wall profile into Python data.
        """
        profile = {
            "wall_type": int(c_profile.wall_type),
            "dir_a_vid": int(c_profile.dir_a_vid),
            "dir_b_vid": int(c_profile.dir_b_vid),
            "pillar_vid": int(c_profile.pillar_vid),
            "step_x": float(c_profile.step_x),
            "step_y": float(c_profile.step_y),
            "offset_a_x": float(c_profile.offset_a_x),
            "offset_a_y": float(c_profile.offset_a_y),
            "offset_b_x": float(c_profile.offset_b_x),
            "offset_b_y": float(c_profile.offset_b_y),
            "offset_p_x": float(c_profile.offset_p_x),
            "offset_p_y": float(c_profile.offset_p_y),
            "grid_divisor": int(c_profile.grid_divisor),
            "drawable_parts": self._load_drawable_parts(int(c_profile.wall_type)),
        }
        return profile

    def _load_drawable_parts(self, wall_type: int) -> list:
        """
        Load drawable part metadata for a wall profile.
        """
        parts = []
        count = self.lib.get_wall_drawable_part_count(wall_type)

        index = 0
        while index < count:
            c_part = CDrawablePart()
            success = self.lib.get_wall_drawable_part_at(wall_type, index, ctypes.byref(c_part))
            if success:
                part_id = self._decode_c_string(c_part.part_id)
                parts.append(part_id)

            index += 1

        return parts

    def _decode_c_string(self, value) -> str:
        """
        Decode a DLL-owned UTF-8 string.
        """
        if value is None:
            return ""

        return value.decode("utf-8")
