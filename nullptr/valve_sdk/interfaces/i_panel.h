#pragma once

class i_panel
{
public:
    const char *get_name(unsigned int vguiPanel)
    {
        typedef const char *(__thiscall* tGetName)(void*, unsigned int);
        return call_vfunction<tGetName>(this, 36)(this, vguiPanel);
    }
#ifdef GetClassName
#undef GetClassName
#endif
    const char *get_class_name(unsigned int vguiPanel)
    {
        typedef const char *(__thiscall* tGetClassName)(void*, unsigned int);
        return call_vfunction<tGetClassName>(this, 37)(this, vguiPanel);
    }
};
