#include "Surfaces/MainSurface.h"
#include "UIFrame/FrameBase/SurfaceManager.h"

#include <vector>
#include <algorithm>

void StartFrame()
{
	SurfaceManager::GetInstance().Start(new MainSurface);
}
