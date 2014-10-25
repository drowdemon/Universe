#include "commProtocol.h"
#include "unit.h"

communicationProtocol::communicationProtocol(unit *wa) : whoAsks(new unit(*wa))
{
}
