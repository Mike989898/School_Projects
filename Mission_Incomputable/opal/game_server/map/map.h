/* A header file for the map module
 *
 *Chris Bertasi, Michael Kilgore, Andrew Laub
 *
 */

#include "../../lib/list/list.h"

#ifndef __MAP_H
#define __MAP_H

//Opens a gtk window with a graphical map display

void openMap(void);

// Updates a map given the current locations of active code drops. drops is a list of code drop structs (cd_t)

void updateDrops(list_t* drops);


// Updates a map given the current locations of active field agents. Agents is a list of agent structs (fa_t)

void updateAgents(list_t* agents);

//Closes the gtk window
void closeMap();

#endif
