#include <Game/Player.h>
#include <Game/Flag.h>

void UT::Player::Reset()
{
	if(flag)flag->Taken(false);
	flag = nullptr;
	shimmer.TurnOff();
}