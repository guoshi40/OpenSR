#include "LuaBindings.h"
#include "libRanger.h"
#include "Log.h"

using namespace Rangers;

std::wstring fromLua(const char *s)
{
    return Rangers::fromUTF8(s);
}

Font* Rangers::getPointer(boost::shared_ptr<Font> sp)
{
    return sp.get();
}

Texture* Rangers::getPointer(boost::shared_ptr<Texture> sp)
{
    return sp.get();
}

AnimatedTexture* Rangers::getPointer(boost::shared_ptr<AnimatedTexture> sp)
{
    return sp.get();
}

void Rangers::luaDebug(std::wstring s)
{
    logger() << LDEBUG << L"Lua: " << s << LEND;
}
