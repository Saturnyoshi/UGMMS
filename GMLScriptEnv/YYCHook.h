
// YYCHook.h - public DLL interface

// Hooks the GameMaker step event using the passed function.
// The function is passed the step event subtype:
// 0 - Normal step
// 1 - Begin step
// 2 - End step
// Returns whether the hook was successful.
bool GMLHookGlobalStep(void(*hook)(int event_subtype));