#include "PrecompiledHeader.h"
#include "UnmanagedWrapper.h"
#include "gui/App.h"
#include "gui/AppSaveStates.h"
#include "GameDatabase.h"
#include "Memory.h"
#include "Cache.h"
#include "IopMem.h"
#include <common/IniInterface.h>
#include "wx/sstream.h"
#include <GS.h>
#include <sstream>
#include <pcsx2/SPU2/spu2.h>
#include <common/SettingsWrapper.h>
#include <pcsx2/gui/wxSettingsInterface.h>

//C++/CLI has various restrictions (no std::mutex for example), so we can't actually include certain headers directly
//What we CAN do is wrap those functions

void UnmanagedWrapper::VANGUARD_EXIT()
{
    wxGetApp().Exit();
}

void UnmanagedWrapper::VANGUARD_LOADSTATE(const wxString &file)
{
    StateCopy_LoadFromFile(file, false);
}

void UnmanagedWrapper::VANGUARD_SAVESTATE(const wxString &file)
{
    StateCopy_SaveToFile(file);
}

void UnmanagedWrapper::VANGUARD_RESUMEEMULATION()
{
    GetCoreThread().Resume();
}

void UnmanagedWrapper::VANGUARD_STOPGAME()
{
    UI_DisableSysShutdown();
    Console.SetTitle("PCSX2 Program Log");
    CoreThread.Reset();
}
void UnmanagedWrapper::VANGUARD_LOADGAME(const wxString &file)
{
    ScopedCoreThreadPause paused_core;
    SysUpdateIsoSrcFile(file);
    AppSaveSettings(); // save the new iso selection; update menus!
    sApp.SysExecute(g_Conf->CdvdSource, wxEmptyString);
}

std::string UnmanagedWrapper::VANGUARD_GETGAMENAME()
{
    // just get the elf name for now; they removed the game_data class and I'm too lazy to fix that right now
    //const wxString newGameKey(SysGetDiscID());
	wxString elfName;
	GetPS2ElfName(elfName);
    //if (!newGameKey.IsEmpty()) {
    //    if (IGameDatabase *GameDB = AppHost_GetGameDatabase()) {
    //        Game_Data game;
    //        if (GameDB->findGame(game, newGameKey)) {
    //            return game.getString("Name").ToStdString();
    //        }
    //    }
    //}
    ////We can't find the name so return the id
    //return newGameKey.ToStdString();
	return elfName.ToStdString();
}

void UnmanagedWrapper::EERAM_POKEBYTE(long long addr, unsigned char val)
{
    memWrite8(static_cast<u32>(addr), val);
}

unsigned char UnmanagedWrapper::EERAM_PEEKBYTE(long long addr)
{
    return memRead8(static_cast<u32>(addr));
}
void UnmanagedWrapper::IOP_POKEBYTE(long long addr, unsigned char val)
{
    iopMemWrite8(static_cast<u32>(addr), val);
}

unsigned char UnmanagedWrapper::IOP_PEEKBYTE(long long addr)
{
    return iopMemRead8(static_cast<u32>(addr));
}
void UnmanagedWrapper::GS_POKEBYTE(long long addr, unsigned char val)
{
    gsWrite8(static_cast<u32>(addr), val);
}

unsigned char UnmanagedWrapper::GS_PEEKBYTE(long long addr)
{
    return gsRead8(static_cast<u32>(addr));
}
void UnmanagedWrapper::SPU_POKEBYTE(long long addr, unsigned char val)
{
	u16 value = 0;
	if ((addr % 2) == 0)
	{
		value = (val << 8) | (SPU_PEEKBYTE(addr + 1));
	}
	else
	{
		addr -= 1;
		value = (SPU_PEEKBYTE(addr) << 8) | val;
	}
	SPU2write(static_cast<u32>(addr), value);
}

unsigned char UnmanagedWrapper::SPU_PEEKBYTE(long long addr)
{
    u16 data = 0;
    u8 byte = 0;
    data = SPU2read(addr);/*
    std::stringstream stream;
    std::string alignment;
    stream << std::hex << addr;
    alignment = stream.str();
    alignment = alignment.back();
    if (alignment == "0" || alignment == "2" || alignment == "4" || alignment == "6" || alignment == "8" || alignment == "a" || alignment == "c" || alignment == "e")
        byte = data & 0xFF;
    if (alignment == "1" || alignment == "3" || alignment == "5" || alignment == "7" || alignment == "9" || alignment == "b" || alignment == "d" || alignment == "f")
        byte = (data >> 8) & 0xFF;*/
    if ((addr % 2) == 0) {
        byte = data & 0xFF;
    } else {
        byte = (data >> 8) & 0xFF;
    }
    return byte;
}

std::string UnmanagedWrapper::VANGUARD_SAVECONFIG()
{

    auto a = new wxFileConfig();
    std::unique_ptr<wxFileConfig> vmini(a);
	wxSettingsInterface vmsaver(vmini.get());
	SettingsSaveWrapper wrapper(vmsaver);
	g_Conf->EmuOptions.LoadSave(wrapper);
    sApp.DispatchVmSettingsEvent((IniSaver&)vmsaver);

    wxStringOutputStream *pStrOutputStream = new wxStringOutputStream();
    a->Save(*pStrOutputStream);
    return pStrOutputStream->GetString().ToStdString();
}

void UnmanagedWrapper::VANGUARD_LOADCONFIG(wxString cfg)
{
    // Load virtual machine options and apply some defaults overtop saved items, which
    // are regulated by the PCSX2 UI.
    
    wxStringInputStream *pStrInStream = new wxStringInputStream(cfg);
    std::unique_ptr<wxFileConfig> vmini(new wxFileConfig(*pStrInStream));
	wxSettingsInterface vmloader(vmini.get());
	SettingsLoadWrapper wrapper(vmloader);
	g_Conf->EmuOptions.LoadSave(wrapper);
	g_Conf->EmuOptions.GS.LimitScalar = g_Conf->EmuOptions.Framerate.NominalScalar;

    if (g_Conf->EnablePresets) {
        g_Conf->IsOkApplyPreset(g_Conf->PresetIndex, true);
    }

    sApp.DispatchVmSettingsEvent((IniLoader&)vmloader);
}