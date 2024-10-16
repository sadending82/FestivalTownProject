#include "DummyClient.h"

DummyClient::DummyClient()
{
	id = INVALID_ID;
	position.x = 0.0;
	position.y = 0.0;
	position.z = 0.0;
	direction.x = 0.0;
	direction.y = 0.0;
	direction.z = 0.0;

	isMove = false;
}

DummyClient::~DummyClient()
{

}
