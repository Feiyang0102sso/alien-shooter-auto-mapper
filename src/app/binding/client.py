"""
Thin client for the Auto Mapper C++ DLL.
"""
import ctypes
from pathlib import Path

from app.binding.structures import CDoor, CSegment, CStandardDoorZConfig, CWallProfile
from app.config import DLL_PATH
from app.logger import logger
from app.project.data import ProjectData


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
            ctypes.c_float,
            ctypes.c_float,
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

    def load_standard_door_z_config(self) -> dict:
        """
        Load standard door z-offset configs from the DLL.
        """
        if not self.load():
            return {}

        configs = {}

        for size in (1, 2):
            config = CStandardDoorZConfig()
            success = self.lib.get_standard_door_z_config(size, ctypes.byref(config))
            if success:
                configs[size] = config

        logger.info(f"Loaded standard door z config: {sorted(configs.keys())}")
        return configs

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
    ) -> bool:
        """
        Generate a .map file through the C++ DLL.
        """
        if not self.load():
            raise FileNotFoundError(f"DLL not found: {self.dll_path}")

        segment_array = self._build_segment_array(project_data.segments)
        door_array = self._build_door_array(project_data.doors)
        output_path_bytes = str(output_path).encode("utf-8")

        success = self.lib.generate_map_from_segments(
            output_path_bytes,
            segment_array,
            len(project_data.segments),
            door_array,
            len(project_data.doors),
            float(project_data.map_size_x),
            float(project_data.map_size_y),
            generate_floor,
            generate_ceiling,
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

    def _build_door_array(self, doors: list):
        """
        Convert Python door tuples into a C array.
        """
        DoorArray = CDoor * len(doors)
        door_array = DoorArray()

        index = 0
        for door in doors:
            door_array[index].x = int(door[0])
            door_array[index].y = int(door[1])
            door_array[index].wall_type = int(door[2])
            door_array[index].direction_type = int(door[3])
            door_array[index].size = int(door[4])
            door_array[index].door_state = int(door[5])
            door_array[index].light_state = int(door[6])
            door_array[index].z_offset = float(door[7])
            index += 1

        return door_array

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
        }
        return profile
