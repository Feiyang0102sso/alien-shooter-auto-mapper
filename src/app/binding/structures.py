"""
ctypes structures that mirror src/auto_mapper/api.h.
"""
import ctypes


class CSegment(ctypes.Structure):
    """
    C-compatible wall segment.
    """

    _fields_ = [
        ("x1", ctypes.c_int),
        ("y1", ctypes.c_int),
        ("x2", ctypes.c_int),
        ("y2", ctypes.c_int),
        ("wall_type", ctypes.c_int),
    ]


class CDoor(ctypes.Structure):
    """
    C-compatible door entry.
    """

    _fields_ = [
        ("x", ctypes.c_int),
        ("y", ctypes.c_int),
        ("wall_type", ctypes.c_int),
        ("direction_type", ctypes.c_int),
        ("size", ctypes.c_int),
        ("door_state", ctypes.c_int),
        ("light_state", ctypes.c_int),
        ("z_offset", ctypes.c_float),
    ]


class CStandardDoorZConfig(ctypes.Structure):
    """
    C-compatible standard door z-offset configuration.
    """

    _fields_ = [
        ("jam_min_z", ctypes.c_float),
        ("jam_max_z", ctypes.c_float),
        ("dead_open_min_z", ctypes.c_float),
        ("dead_open_max_z", ctypes.c_float),
    ]


class CWallProfile(ctypes.Structure):
    """
    C-compatible wall profile from the C++ engine.
    """

    _fields_ = [
        ("wall_type", ctypes.c_int),
        ("dir_a_vid", ctypes.c_int),
        ("dir_b_vid", ctypes.c_int),
        ("pillar_vid", ctypes.c_int),
        ("step_x", ctypes.c_float),
        ("step_y", ctypes.c_float),
        ("offset_a_x", ctypes.c_float),
        ("offset_a_y", ctypes.c_float),
        ("offset_b_x", ctypes.c_float),
        ("offset_b_y", ctypes.c_float),
        ("offset_p_x", ctypes.c_float),
        ("offset_p_y", ctypes.c_float),
        ("grid_divisor", ctypes.c_int),
    ]


class CDrawablePart(ctypes.Structure):
    """
    C-compatible drawable part metadata.
    """

    _fields_ = [
        ("part_id", ctypes.c_char_p),
    ]
