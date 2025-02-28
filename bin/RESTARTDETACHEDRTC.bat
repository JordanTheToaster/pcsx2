@echo off

taskkill /F /IM pcsx2x64.exe > nul 2>&1
taskkill /F /IM WerFault.exe > nul 2>&1

start pcsx2x64.exe
exit