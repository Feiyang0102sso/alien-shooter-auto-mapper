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
