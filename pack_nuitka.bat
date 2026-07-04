@echo off
taskkill /f /im AutoMapper.exe 2>nul

call .\.venv\Scripts\activate.bat
pip install nuitka ordered-set
python pack_nuitka.py
pause
