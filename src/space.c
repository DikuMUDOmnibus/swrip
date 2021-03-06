/***************************************************************************
*                           STAR WARS REALITY 1.0                          *
*--------------------------------------------------------------------------*
* Star Wars Reality Code Additions and changes from the Smaug Code         *
* copyright (c) 1997 by Sean Cooper                                        *
* -------------------------------------------------------------------------*
* Starwars and Starwars Names copyright(c) Lucas Film Ltd.                 *
*--------------------------------------------------------------------------*
* SMAUG 1.0 (C) 1994, 1995, 1996 by Derek Snider                           *
* SMAUG code team: Thoric, Altrag, Blodkai, Narn, Haus,                    *
* Scryn, Rennard, Swordbearer, Gorog, Grishnakh and Tricops                *
* ------------------------------------------------------------------------ *
* Merc 2.1 Diku Mud improvments copyright (C) 1992, 1993 by Michael        *
* Chastain, Michael Quan, and Mitchell Tse.                                *
* Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,          *
* Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.     *
* ------------------------------------------------------------------------ *
*		                Space Module    			   *   
****************************************************************************/

#include <math.h>
#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include "mud.h"

SHIP_DATA * first_ship;
SHIP_DATA * last_ship;

MISSILE_DATA * first_missile;
MISSILE_DATA * last_missile;

SPACE_DATA * first_spaceobject;
SPACE_DATA * last_spaceobject;

int bus_pos =0;
int bus_planet =0;
int bus2_planet = 8;
int bus3_planet = 11;
int bus4_planet = 3;
int turbocar_stop =0;
int corus_shuttle =0;
int baycount = 0;

int currrentalvnum = 6000; // increments as rentals are created

#define RENTALSTORAGEROOM 44 // Room where unused rentals are stored

#define DEGMULTI 149760

#define MAX_COORD 15000000
#define MAX_COORD_S 14000000
#define MAX_STATION    10
#define MAX_BUS_STOP 14

#define SHIPDAM_MINPLATFORM	100
#define SHIPDAM_MAXPLATFORM	250
#define SHIPDAM_MINCAP		100
#define SHIPDAM_MAXCAP		200
#define SHIPDAM_MINDEF		5
#define SHIPDAM_MAXDEF		10

#define STOP_PLANET     202
#define STOP_SHIPYARD   32015

#define SENATEPAD       32055
#define OUTERPAD        2064

struct	serin_shuttle
{
    char 		name	[20];	
    int			startingloc;
    int			cockpitvnum;
    int			planetloc;
    int		const  	bus_vnum [12];
    char * 	const   bus_stop [12];
};

/* New Serin system, makes it much easier to add new ones - Darrik Vequir, May, 01 */
/* MAX_BUS defined in mud.h */

struct serin_shuttle serin[MAX_BUS] =
{
{
  "Pluogus", 0, 32140, 0,
    { 201, 21943, 28613, 21100, 28038, 10500, 201 },
    { "Coruscant", "Corulag", "Kashyyyk", "Mon Calamari", "Gamorr", "Corellia", "Coruscant" }

},
{
  "Tocca", 0, 32410, 0,
    { 5550, 21943, 10500, 29001, 11800, 4100, 23082, 5550 },
    { "Ord Mantell", "Corulag", "Corellia", "Adari", "Roche", "Sullust", "Kuat", "Ord Mantell" }

},
{
  "Cassia", 0, 32425, 0,
    { 28247, 10500, 5200, 9729, 31872, 822, 28038, 10500, 28247 },
    { "Byss", "Corellia", "Af'el", "Uvena III", "Tatooine", "Ryloth", "Gamorr", "Corellia", "Byss" }

},
{
  "Siego", 0, 32435, 0,
    { 201, 10500, 6264, 11206, 2300, 4305, 28247, 201 },
    { "Coruscant", "Corellia", "Bespin", "Hoth", "Endor", "Wroona", "Byss", "Coruscant"}

},
{
  "Amose", 0, 32445, 0,
    { 2300, 6264, 4100, 31872, 9729, 28038, 21100, 5550, 2300 },
    { "Endor", "Bespin", "Sullust", "Tatooine", "Uvena III", "Gamorr", "Mon Calamari", "Ord Mantell", "Endor"}

},
{
  "Edena", 0, 32440, 0,
    { 28247, 201, 21943, 23082, 10500, 29001, 4305, 28247 },
    { "Byss", "Coruscant", "Corulag", "Kuat", "Corellia", "Adari", "Wroona", "Byss"}

},
{
  "Odani", 0, 32450, 0,
    { 4305, 11800, 28613, 5550, 23082, 10500, 29001, 4305 },
    { "Wroona", "Roche", "Kashyyyk", "Ord Mantell", "Kuat", "Corellia", "Adari", "Wroona"}

}
};


int     const   station_vnum [MAX_STATION] =
{
    215 , 216 , 217 , 218 , 219 , 220 , 221 ,222 , 223 , 224
};

char *  const   station_name [MAX_STATION] =
{
   "Menari Spaceport" , "Skydome Botanical Gardens" , "Grand Towers" ,
   "Grandis Mon Theater" , "Palace Station" , "Great Galactic Museum" ,
   "College Station" , "Holographic Zoo of Extinct Animals" ,
   "Dometown Station " , "Monument Plaza"
};

int     const   bus_vnum [MAX_BUS_STOP] =
{
    201 ,  21100 , 29001 , 28038 , 31872 , 822 , 6264, 28613 , 3060 , 28247, 32297, 2065, 10500, 11206
};
int     const   bus_vnum2 [MAX_BUS_STOP] =
{
    201, 11206 ,  10500 , 2065 , 32297 , 28247 , 3060 , 28613 , 6264, 822 , 31872, 28038, 29001, 21100
};

char *  const   bus_stop [MAX_BUS_STOP+1] =
{
   "Coruscant",
   "Mon Calamari", "Adari", "Gamorr", "Tatooine" , "Ryloth", "Bespin",
   "Kashyyyk", "Endor", "Byss", "Cloning Facilities", "Kuat", "Corellia", "Hoth", "Coruscant"  /* last should always be same as first */
};

char *  const bus_stoprev [MAX_BUS_STOP+1] =
{
   "Coruscant",
   "Hoth", "Corellia", "Kuat", "Cloning Facilities" , "Byss" , "Endor", "Kashyyyk",
   "Bespin", "Ryloth", "Tatooine", "Gamorr", "Adari", "Mon Calamari", "Coruscant"  /* last should always be same as first */
};

RENTAL_DATA rentals[MAX_RENTALS] =
{
  {
  HOPPER_RENTAL, 6030
  },
  {
  TWING_RENTAL, 32200
  }
};

// TURBOLASER_TURRET, QUAD_TURRET, MAX_TURRET_TYPE, ION_TURRET, MISSILE_TURRET, TRACTOR_TURRET

struct turret_type turretstats[MAX_TURRET_TYPE] =
{
  { TURBOLASER_TURRET, 100, 250,
    "Turbo-laser Mount",
    "You are hit by turbolasers from %s!",
    "Turboasers fire from %s, hitting %s.",
    "The turbo-laser fires at %s, scoring a hit.",
    "Turbolasers fire from %s at you but miss.",
    "Turbolasers fire from %s at %s but miss.",
    "The turbo-laser fires at %s and misses."
  },
  { QUAD_TURRET, 25, 50,
    "Quad-cannon Mount",
    "You are hit by a quad cannon from %s!",
    "Laser cannon fire from %s, hitting %s.",
    "The laser cannon fires at %s, scoring a hit.",
    "Laser cannon fire from %s at flash by you but miss.",
    "Laser cannon fire from %s flash at %s but miss.",
    "The laser cannon fires at %s and misses."
  },
  { ION_TURRET, -25, -50,
    "Ion-cannon Mount",
    "You are hit by an ion cannon from %s!",
    "Ion cannon fire from %s, hitting %s.",
    "The ion cannon fires at %s, scoring a hit.",
    "Ion cannon fire from %s at flash by you but miss.",
    "Ion cannon fire from %s flash at %s but miss.",
    "The ion cannon fires at %s and misses."
  },
  { MISSILE_TURRET, 1, 1,
    "Missile Launcher Mount",
    "You are launched on by a missile turret from %s!",
    "A missile launcher fires from %s.",
    "The launcher fires at %s.",
    "Not used",
    "Not used",
    "The launcher attempts to launch on %s but misfires."
  },
  { TRACTOR_TURRET, 1, 1,
    "Tractor Beam Mount",
    "You are captured by a tractor beam from %s!",
    "%s captures %s with a tractor beam.",
    "The tractor beam locks on to %s.",
    "A tractor beam from %s attempts to capture you but loses lock.",
    "A tractor beam from %s attempts to capture %s, but loses lock.",
    "The tractor beam attempts to capture %s but loses lock."
  }
};


/* local routines */
void	fread_ship	args( ( SHIP_DATA *ship, FILE *fp ) );
void	write_ship_list	args( ( void ) );
void    fread_spaceobject      args( ( SPACE_DATA *spaceobject, FILE *fp ) );
bool    load_spaceobject  args( ( char *spaceobjectfile ) );
void    write_spaceobject_list args( ( void ) );
void    resetship args( ( SHIP_DATA *ship ) );
void    approachland args( ( SHIP_DATA *ship, char *arg ) );
void    landship args( ( SHIP_DATA *ship, char *arg ) );
void    launchship args( ( SHIP_DATA *ship ) );
bool    land_bus args( ( SHIP_DATA *ship, int destination ) );
void    launch_bus args( ( SHIP_DATA *ship ) );
void    echo_to_room_dnr args( ( int ecolor , ROOM_INDEX_DATA *room ,  char *argument ) );
ch_ret drive_ship( CHAR_DATA *ch, SHIP_DATA *ship, EXIT_DATA  *exit , int fall );
bool is_facing( SHIP_DATA *ship , SHIP_DATA *target );
void sound_to_ship( SHIP_DATA *ship , char *argument );
void modtrainer( SHIP_DATA *ship, sh_int shipclass );
void makedebris( SHIP_DATA *ship );
bool space_in_range_h( SHIP_DATA *ship, SPACE_DATA *space);
void dumpship( SHIP_DATA *ship, int destination );
void shipdelete( SHIP_DATA *ship, bool shiplist );
void storeship( SHIP_DATA *ship );
/* from comm.c */
bool    write_to_descriptor     args( ( int desc, char *txt, int length ) );

ROOM_INDEX_DATA *generate_exit( ROOM_INDEX_DATA *in_room, EXIT_DATA **pexit );


void echo_to_room_dnr ( int ecolor , ROOM_INDEX_DATA *room ,  char *argument )
{
    CHAR_DATA *vic;

    if ( room == NULL )
    	return;

    for ( vic = room->first_person; vic; vic = vic->next_in_room )
    {
	set_char_color( ecolor, vic );
	send_to_char( argument, vic );
    }
}


bool  land_bus( SHIP_DATA *ship, int destination )
{
    char buf[MAX_STRING_LENGTH];
    
    if ( !ship_to_room( ship , destination ) )
    {
       return FALSE;
    }
    echo_to_ship( AT_YELLOW , ship , "You feel a slight thud as the ship sets down on the ground.");
    ship->location = destination;
    ship->lastdoc = ship->location;
    ship->shipstate = SHIP_LANDED;
    if (ship->spaceobject)
        ship_from_spaceobject( ship, ship->spaceobject );
    sprintf( buf, "%s lands on the platform.", ship->name );
    echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    sprintf( buf , "The hatch on %s opens." , ship->name);
    echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch opens." );
    ship->hatchopen = TRUE;
    sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
    sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );
    return TRUE;
}

void    launch_bus( SHIP_DATA *ship )
{
      char buf[MAX_STRING_LENGTH];

      sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
       sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );
      sprintf( buf , "The hatch on %s closes and it begins to launch." , ship->name);
      echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
      echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch slides shut." );
      ship->hatchopen = FALSE;
      extract_ship( ship );
      echo_to_ship( AT_YELLOW , ship , "The ship begins to launch.");
      ship->location = 0;
      ship->shipstate = SHIP_READY;
}


bool is_bus_stop( int vnum )
{
  int bus, stop;

  for ( bus = 0; bus < MAX_BUS; bus++ )
    for( stop = 0; ( stop < 12 || serin[bus].bus_vnum[stop] != '\0') ; stop++ )
      if ( vnum == serin[bus].bus_vnum[stop] )
        return TRUE;
  return FALSE;
}


void update_traffic( )
{
    SHIP_DATA *shuttle, *senate;
    SHIP_DATA *turbocar;
    char       buf[MAX_STRING_LENGTH];

    shuttle = ship_from_cockpit( ROOM_CORUSCANT_SHUTTLE );
    senate = ship_from_cockpit( ROOM_SENATE_SHUTTLE );
    if ( senate != NULL && shuttle != NULL )
    {
        switch (corus_shuttle)
        {
             default:
                corus_shuttle++;
                break;

             case 0:
                land_bus( shuttle , STOP_PLANET );
                land_bus( senate , SENATEPAD );
                corus_shuttle++;
                echo_to_ship( AT_CYAN , shuttle , "Welcome to Menari Spaceport." );
                echo_to_ship( AT_CYAN , senate , "Welcome to Kuat Shipyard." );
                break;

             case 4:
                launch_bus( shuttle );
                launch_bus( senate );
                corus_shuttle++;
                break;

             case 5:
                land_bus( shuttle , STOP_SHIPYARD );
                land_bus( senate , OUTERPAD );
                echo_to_ship( AT_CYAN , shuttle , "Welcome to Coruscant Shipyard." );
                echo_to_ship( AT_CYAN , senate , "Welcome to the planet of Kuat." );
                corus_shuttle++;
                break;
                
             case 9:
                launch_bus( shuttle );
                launch_bus( senate );
                corus_shuttle++;
                break;

        }
        
        if ( corus_shuttle >= 10 )
              corus_shuttle = 0;
    }
    
    turbocar = ship_from_cockpit( ROOM_CORUSCANT_TURBOCAR );
    if ( turbocar != NULL )
    {
      	sprintf( buf , "The turbocar doors close and it speeds out of the station.");  
      	echo_to_room( AT_YELLOW , get_room_index(turbocar->location) , buf );
      	extract_ship( turbocar );
      	turbocar->location = 0;
       	ship_to_room( turbocar , station_vnum[turbocar_stop] );
       	echo_to_ship( AT_YELLOW , turbocar , "The turbocar makes a quick journey to the next station."); 
       	turbocar->location = station_vnum[turbocar_stop];
       	turbocar->lastdoc = turbocar->location;
       	turbocar->shipstate = SHIP_LANDED;
       	if (turbocar->spaceobject)
          ship_from_spaceobject( turbocar, turbocar->spaceobject );  
    	sprintf( buf, "A turbocar pulls into the platform and the doors slide open.");
    	echo_to_room( AT_YELLOW , get_room_index(turbocar->location) , buf );
    	sprintf( buf, "Welcome to %s." , station_name[turbocar_stop] );
    	echo_to_ship( AT_CYAN , turbocar , buf );
        turbocar->hatchopen = TRUE;
        
        turbocar_stop++;
        if ( turbocar_stop >= MAX_STATION )
           turbocar_stop = 0;
    }
       
}

void update_bus( )
{
    SHIP_DATA * ship[MAX_BUS];
/*    SHIP_DATA *ship2;
    SHIP_DATA *ship3;
    SHIP_DATA *ship4;
*/
    SHIP_DATA *target;
    int        destination, bus;
    char       buf[MAX_STRING_LENGTH];

   for( bus = 0; bus < MAX_BUS; bus++ )
     ship[bus] = ship_from_cockpit( serin[bus].cockpitvnum );

    switch (bus_pos)
    {

       case 0:
            for( bus = 0; bus < MAX_BUS; bus++ )
            {
            if( ship_from_cockpit( serin[bus].cockpitvnum ) == NULL )
              continue;
              target = ship_from_hanger( serin[bus].bus_vnum[serin[bus].planetloc] );
              if ( target != NULL && !target->spaceobject )
              {
                sprintf( buf,  "An electronic voice says, 'Cannot land at %s ... it seems to have dissapeared.'", serin[bus].bus_stop[serin[bus].planetloc] );
                echo_to_ship( AT_CYAN , ship[bus] , buf );
                bus_pos = 5;
              }
            }
/*              
            target = ship_from_hanger( bus_vnum[bus_planet] );
            if ( target != NULL && !target->spaceobject )
            {
               sprintf( buf,  "An electronic voice says, 'Cannot land at %s ... it seems to have dissapeared.'", bus_stop[bus_planet] );
               echo_to_ship( AT_CYAN , ship , buf );
               bus_pos = 5;
            }

            target = ship_from_hanger( bus_vnum[bus2_planet] );
            if ( target != NULL && !target->spaceobject )
            {
               sprintf( buf,  "An electronic voice says, 'Cannot land at %s ... it seems to have dissapeared.'", bus_stop[bus_planet] );
               echo_to_ship( AT_CYAN , ship2 , buf );
               bus_pos = 5;
            }
            target = ship_from_hanger( bus_vnum2[bus3_planet] );
            if ( target != NULL && !target->spaceobject )
            {
               sprintf( buf,  "An electronic voice says, 'Cannot land at %s ... it seems to have dissapeared.'", bus_stoprev[bus_planet] );
               echo_to_ship( AT_CYAN , ship3 , buf );
               bus_pos = 5;
            }
            target = ship_from_hanger( bus_vnum2[bus4_planet] );
            if ( target != NULL && !target->spaceobject )
            {
               sprintf( buf,  "An electronic voice says, 'Cannot land at %s ... it seems to have dissapeared.'", bus_stoprev[bus_planet] );
               echo_to_ship( AT_CYAN , ship4 , buf );
               bus_pos = 5;
            }
*/
            bus_pos++;
            break;

       case 6:

/*
            launch_bus ( ship );
            launch_bus ( ship2 );
            launch_bus ( ship3 );
            launch_bus ( ship4 );
*/  
	    for( bus = 0; bus < MAX_BUS; bus++ )
	    {
            if( ship_from_cockpit( serin[bus].cockpitvnum ) == NULL )
              continue;
	      launch_bus( ship[bus] );
	    }
            bus_pos++;
            break;

       case 7:
/*          echo_to_ship( AT_YELLOW , ship , "The ship lurches slightly as it makes the jump to lightspeed.");
            echo_to_ship( AT_YELLOW , ship2 , "The ship lurches slightly as it makes the jump to lightspeed.");
            echo_to_ship( AT_YELLOW , ship3 , "The ship lurches slightly as it makes the jump to lightspeed.");
            echo_to_ship( AT_YELLOW , ship4 , "The ship lurches slightly as it makes the jump to lightspeed.");
*/
	    for( bus = 0; bus < MAX_BUS; bus++ )
	    {
            if( ship_from_cockpit( serin[bus].cockpitvnum ) == NULL )
              continue;
    	
              echo_to_ship( AT_YELLOW , ship[bus] , "The ship lurches slightly as it makes the jump to lightspeed.");
            }
            bus_pos++;
            break;

       case 9:

/*            echo_to_ship( AT_YELLOW , ship , "The ship lurches slightly as it comes out of hyperspace..");
            echo_to_ship( AT_YELLOW , ship2 , "The ship lurches slightly as it comes out of hyperspace..");
            echo_to_ship( AT_YELLOW , ship3 , "The ship lurches slightly as it comes out of hyperspace..");
            echo_to_ship( AT_YELLOW , ship4 , "The ship lurches slightly as it comes out of hyperspace..");
*/
	    for( bus = 0; bus < MAX_BUS; bus++ )
	    {
            if( ship_from_cockpit( serin[bus].cockpitvnum ) == NULL )
              continue;
	      echo_to_ship( AT_YELLOW , ship[bus] , "The ship lurches slightly as it comes out of hyperspace.");
	    }
	      
            bus_pos++;
            break;

       case 1:

	    for( bus = 0; bus < MAX_BUS; bus++ )
          {
            if( ship_from_cockpit( serin[bus].cockpitvnum ) == NULL )
              continue;
              destination = serin[bus].bus_vnum[serin[bus].planetloc];
            if ( !land_bus( ship[bus], destination ) )
            {
               sprintf( buf, "An electronic voice says, 'Oh My, %s seems to have dissapeared.'" , serin[bus].bus_stop[serin[bus].planetloc] );
               echo_to_ship( AT_CYAN , ship[bus] , buf );
               echo_to_ship( AT_CYAN , ship[bus] , "An electronic voice says, 'Alderaan was bad enough... Landing aborted.'");
            }
            else
            {
               sprintf( buf,  "An electronic voice says, 'Welcome to %s'" , serin[bus].bus_stop[serin[bus].planetloc] );
               echo_to_ship( AT_CYAN , ship[bus] , buf);
               echo_to_ship( AT_CYAN , ship[bus] , "It continues, 'Please exit through the main ramp. Enjoy your stay.'");
            }
          }
/*          destination = bus_vnum[bus2_planet];
            if ( !land_bus( ship2, destination ) )
            {
               sprintf( buf, "An electronic voice says, 'Oh My, %s seems to have dissapeared.'" , bus_stop[bus2_planet] );
               echo_to_ship( AT_CYAN , ship2 , buf );
               echo_to_ship( AT_CYAN , ship2 , "An electronic voice says, 'I do hope it wasn't a superlaser. Landing aborted.'");
            }
            else
            {
               sprintf( buf,  "An electronic voice says, 'Welcome to %s'" , bus_stop[bus2_planet] );
               echo_to_ship( AT_CYAN , ship2 , buf);
               echo_to_ship( AT_CYAN , ship2 , "It continues, 'Please exit through the main ramp. Enjoy your stay.'");
            }
            destination = bus_vnum2[bus3_planet];
            if ( !land_bus( ship3, destination ) )
            {
               sprintf( buf, "An electronic voice says, 'Oh My, %s seems to have dissapeared.'" , bus_stoprev[bus3_planet] );
               echo_to_ship( AT_CYAN , ship3 , buf );
               echo_to_ship( AT_CYAN , ship3 , "An electronic voice says, 'I do hope it wasn't a superlaser. Landing aborted.'");
            }
            else
            {
               sprintf( buf,  "An electronic voice says, 'Welcome to %s'" , bus_stoprev[bus3_planet] );
               echo_to_ship( AT_CYAN , ship3 , buf);
               echo_to_ship( AT_CYAN , ship3 , "It continues, 'Please exit through the main ramp. Enjoy your stay.'");
            }
            destination = bus_vnum2[bus4_planet];
            if ( !land_bus( ship4, destination ) )
            {
               sprintf( buf, "An electronic voice says, 'Oh My, %s seems to have dissapeared.'" , bus_stoprev[bus4_planet] );
               echo_to_ship( AT_CYAN , ship4 , buf );
               echo_to_ship( AT_CYAN , ship4 , "An electronic voice says, 'I do hope it wasn't a superlaser. Landing aborted.'");
            }
            else
            {
               sprintf( buf,  "An electronic voice says, 'Welcome to %s'" , bus_stoprev[bus4_planet] );
               echo_to_ship( AT_CYAN , ship4 , buf);
               echo_to_ship( AT_CYAN , ship4 , "It continues, 'Please exit through the main ramp. Enjoy your stay.'");
            }
*/
            bus_pos++;
            break;

       case 5:
         for( bus = 0; bus < MAX_BUS; bus++)
         {
            if( ship_from_cockpit( serin[bus].cockpitvnum ) == NULL )
              continue;

            sprintf( buf, "It continues, 'Next stop, %s'" , serin[bus].bus_stop[serin[bus].planetloc+1] );
            echo_to_ship( AT_CYAN , ship[bus] , "An electronic voice says, 'Preparing for launch.'");
            echo_to_ship( AT_CYAN , ship[bus] , buf);
         }
/*          sprintf( buf, "It continues, 'Next stop, %s'" , bus_stop[bus2_planet+1] );
            echo_to_ship( AT_CYAN , ship2 , "An electronic voice says, 'Preparing for launch.'");
            echo_to_ship( AT_CYAN , ship2 , buf);
            sprintf( buf, "It continues, 'Next stop, %s'" , bus_stoprev[bus3_planet+1] );
            echo_to_ship( AT_CYAN , ship3 , "An electronic voice says, 'Preparing for launch.'");
            echo_to_ship( AT_CYAN , ship3 , buf);
            sprintf( buf, "It continues, 'Next stop, %s'" , bus_stoprev[bus4_planet+1] );
            echo_to_ship( AT_CYAN , ship4 , "An electronic voice says, 'Preparing for launch.'");
            echo_to_ship( AT_CYAN , ship4 , buf);
*/          bus_pos++;
            break;

       default:
            bus_pos++;
            break;
    }

    if ( bus_pos >= 10 )
    {
        bus_pos = 0;
	for( bus = 0; bus < MAX_BUS; bus++ )
	{
	  serin[bus].planetloc++;
          if( serin[bus].bus_vnum[serin[bus].planetloc] == serin[bus].bus_vnum[0] )
            serin[bus].planetloc = 0;
        }
/*        bus_planet++;
        bus2_planet++;
        bus3_planet++;
        bus4_planet++;
*/
    }
       
/*  if ( bus_planet >= MAX_BUS_STOP )
       bus_planet = 0;
    if ( bus2_planet >= MAX_BUS_STOP )
       bus2_planet = 0;
    if ( bus3_planet >= MAX_BUS_STOP )
       bus3_planet = 0;
    if ( bus4_planet >= MAX_BUS_STOP )
       bus4_planet = 0;
*/
}

void move_ships( )
{
   SHIP_DATA *ship;
   MISSILE_DATA *missile;
   MISSILE_DATA *m_next;
   SHIP_DATA *target;
   SPACE_DATA *spaceobj;
   float dx, dy, dz, change;
   char buf[MAX_STRING_LENGTH];
   CHAR_DATA *ch;
   bool ch_found = FALSE;
   sh_int crashsun = 0;
   int speed;
  
   for( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
   {
     if ( spaceobj->speed > 0 )
        {
          
          change = sqrt( spaceobj->hx*spaceobj->hx + spaceobj->hy*spaceobj->hy + spaceobj->hz*spaceobj->hz ); 

           if (change > 0)
           {
             dx = spaceobj->hx/change;     
             dy = spaceobj->hy/change;
             dz = spaceobj->hz/change;
             spaceobj->xpos += (dx * spaceobj->speed/5);
             spaceobj->ypos += (dy * spaceobj->speed/5);
             spaceobj->zpos += (dz * spaceobj->speed/5);
           }
           
        } 

     if( spaceobj->xpos > MAX_COORD )
       spaceobj->xpos = -MAX_COORD_S;
     if( spaceobj->ypos > MAX_COORD )
       spaceobj->ypos = -MAX_COORD_S;
     if( spaceobj->zpos > MAX_COORD )
       spaceobj->zpos = -MAX_COORD_S;
     if( spaceobj->xpos < -MAX_COORD )
       spaceobj->xpos = MAX_COORD_S;
     if( spaceobj->ypos < -MAX_COORD )
       spaceobj->ypos = MAX_COORD_S;
     if( spaceobj->zpos < -MAX_COORD )
       spaceobj->zpos = MAX_COORD_S;

    }
   	

   for ( missile = first_missile; missile; missile = m_next )
   {
           m_next = missile->next;
           
              ship = missile->fired_from;
              target = missile->target;
              
              if ( target->spaceobject && missile_in_range( ship, missile ) )
              {
                if ( missile->mx < target->vx ) 
                  missile->mx += UMIN( missile->speed/5 , target->vx - missile->mx );
                else if ( missile->mx > target->vx ) 
                  missile->mx -= UMIN( missile->speed/5 , missile->mx - target->vx );  
                if ( missile->my < target->vy )
                  missile->my += UMIN( missile->speed/5 , target->vy - missile->my );
                else if ( missile->my > target->vy ) 
                  missile->my -= UMIN( missile->speed/5 , missile->my - target->vy );
                if ( missile->mz < target->vz )
                  missile->mz += UMIN( missile->speed/5 , target->vz - missile->mz );
                else if ( missile->mz > target->vz ) 
                  missile->mz -= UMIN( missile->speed/5 , missile->mz - target->vz );  
              
                if ( abs( (int) ( missile->mx-target->vx )) <= 20 && abs( (int) ( missile->mx-target->vx )) >= -20
                && abs( (int) ( missile->my-target->vy )) <= 20 && abs( (int) ( missile->my-target->vy )) >= -20
                && abs((int) ( missile->mz-target->vz )) <= 20 && abs( (int) ( missile->mz-target->vz )) >= -20 )
                {  
                   if ( target->chaff_released <= 0)
                   { 
                    echo_to_room( AT_YELLOW , get_room_index(ship->gunseat), "Your missile hits its target dead on!" );
                    echo_to_cockpit( AT_BLOOD, target, "The ship is hit by a missile.");
                    echo_to_ship( AT_RED , target , "A loud explosion shakes thee ship violently!" );
                    sprintf( buf, "You see a small explosion as %s is hit by a missile" , target->name );
                    echo_to_system( AT_ORANGE , target , buf , ship );
                    for ( ch = first_char; ch; ch = ch->next )
                      if ( !IS_NPC( ch ) && nifty_is_name( missile->fired_by, ch->name ) )
                      {
                        ch_found = TRUE;
                        damage_ship_ch( target , 30+missile->missiletype*missile->missiletype*30 , 
                           50+missile->missiletype*missile->missiletype*missile->missiletype*50 , ch );
                      }
                    if ( !ch_found )
                       damage_ship( target , ship, 20+missile->missiletype*missile->missiletype*20 , 
                           30+missile->missiletype*missile->missiletype*ship->missiletype*30 );   
                    extract_missile( missile );
                   }
                   else
                   {
                    echo_to_room( AT_YELLOW , get_room_index(ship->gunseat), "Your missile explodes harmlessly in a cloud of chaff!" );
                    echo_to_cockpit( AT_YELLOW, target, "A missile explodes in your chaff.");
                    extract_missile( missile );
                   }
                   continue;
                }
                else
                {
                   missile->age++;
                   if (missile->age >= 50)
                   {
                      extract_missile( missile );
                      continue;
                   }
                }
              }
              else
              { 
                   missile->age++;
                   if (missile->age >= 50)
                      extract_missile( missile );
              }

   }
   
   for ( ship = first_ship; ship; ship = ship->next )
   {
     
    if ( ship->shipstate != SHIP_HYPERSPACE && ship->accel && ship->currspeed != ship->goalspeed )
    {
     if( ship->goalspeed > ship->currspeed )
     {
       ship->currspeed += ship->accel;
       if ( ship->currspeed > ship->goalspeed )
         ship->currspeed = ship->goalspeed;
     }
     else if( ship->goalspeed < ship->currspeed )
     {
       ship->currspeed -= ship->accel;
       if ( ship->currspeed < ship->goalspeed )
         ship->currspeed = ship->goalspeed;
     }
     else if ( ship->currspeed <= 0 && ship->goalspeed <= 0 && ship->accel )
     {
       ship->accel = 0; 
       echo_to_cockpit( AT_YELLOW, ship, "Your computer beeps as the ship reaches a stop.");
       sprintf( buf, "%s slows to a stop." , ship->name );
       echo_to_system( AT_ORANGE , ship , buf , NULL );
     }
     else if ( ship->currspeed >= ship->realspeed && ship->goalspeed >= ship->realspeed && ship->accel )
     {
       ship->accel = 0; 
       echo_to_cockpit( AT_YELLOW, ship, "Your computer beeps as the ship reaches max speed.");
       sprintf( buf, "%s steadies its speed." , ship->name );
       echo_to_system( AT_ORANGE , ship , buf , NULL );
     }
     else if( ship->goalspeed == ship->currspeed && ship->accel )
     {
       ship->accel = 0; 
       echo_to_cockpit( AT_YELLOW, ship, "Your computer beeps as the ship reaches designated speed.");
       sprintf( buf, "%s steadies its speed." , ship->name );
       echo_to_system( AT_ORANGE , ship , buf , NULL );
     }
    }

     if ( !ship->spaceobject )
        continue;
        
     if( ship->shipstate == SHIP_LANDED && ship->spaceobject )
       ship->shipstate = SHIP_READY;

     if ( ship->currspeed > 0 && ship->shipstate != SHIP_LAND && ship->shipstate != SHIP_LAND_2)
        {
          
          change = sqrt( ship->hx*ship->hx + ship->hy*ship->hy + ship->hz*ship->hz ); 

           if (change > 0)
           {
             dx = ship->hx/change;     
             dy = ship->hy/change;
             dz = ship->hz/change;
             ship->vx += (dx * ship->currspeed/5);
             ship->vy += (dy * ship->currspeed/5);
             ship->vz += (dz * ship->currspeed/5);
           }
           
        } 

     if ( ship->tractoredby && ( ship->tractoredby->shipclass <= ship->shipclass || ship->shipclass == SHIP_DEBRIS ) )
     {
	    ship->tractoredby->currspeed = (ship->tractoredby->mod->tractorbeam)/4;
	    ship->tractoredby->currspeed = UMAX( ship->tractoredby->currspeed, ship->currspeed );
	    ship->tractoredby->hx = ship->vx - ship->tractoredby->vx;
	    ship->tractoredby->hy = ship->vy - ship->tractoredby->vy;
	    ship->tractoredby->hz = ship->vz - ship->tractoredby->vz;

/*
          float hx,hy,hz;

	  hx = ship->tractoredby->vx - ship->vx;          
	  hy = ship->tractoredby->vy - ship->vy;          
	  hz = ship->tractoredby->vz - ship->vz;          
          
          change = sqrt( hx*hx + hy*hy + hz*hz ); 

           if (change > 0)
           {
             dx = hx/change;     
             dy = hy/change;
             dz = hz/change;
             ship->vx += UMIN( hx, (dx * (ship->tractoredby->currspeed+25)/5));
             ship->vy += UMIN( hy, (dy * (ship->tractoredby->currspeed+25)/5));
             ship->vz += UMIN( hz, (dz * (ship->tractoredby->currspeed+25)/5));
           }

  */         
        } 

     if ( ship->tractoredby && ship->tractoredby->shipclass > ship->shipclass )
     {

	    ship->currspeed = ship->tractoredby->mod->tractorbeam/4;
	    ship->currspeed = UMAX( ship->currspeed, ship->tractoredby->currspeed );
	    ship->hx = ship->tractoredby->vx - ship->vx;
	    ship->hy = ship->tractoredby->vy - ship->vy;
	    ship->hz = ship->tractoredby->vz - ship->vz;

/*
          float hx,hy,hz;

	  hx = ship->tractored->vx - ship->vx;          
	  hy = ship->tractored->vy - ship->vy;          
	  hz = ship->tractored->vz - ship->vz;          
          
          change = sqrt( hx*hx + hy*hy + hz*hz ); 

           if (change > 0)
           {
             dx = hx/change;     
             dy = hy/change;
             dz = hz/change;
             ship->vx += UMIN( hx, (dx * (ship->tractored->currspeed+25)/5));
             ship->vy += UMIN( hy, (dy * (ship->tractored->currspeed+25)/5));
             ship->vz += UMIN( hz, (dz * (ship->tractored->currspeed+25)/5));
           }
*/
       }    


	if ( ship->tractoredby )
	{
 	  if ( abs( (int) ( ship->vx - ship->tractoredby->vx )) < 10 &&
               abs( (int) ( ship->vy - ship->tractoredby->vy )) < 10 &&
               abs( (int) ( ship->vz - ship->tractoredby->vz )) < 10 )
          {
            if ( ship->shipclass < ship->tractoredby->shipclass )
            {
              ship->vx = ship->tractoredby->vx;
              ship->vy = ship->tractoredby->vy;
              ship->vz = ship->tractoredby->vz;
            }
            if ( ship->shipclass >= ship->tractoredby->shipclass )	
	    {
	    	ship->tractoredby->vx = ship->vx;
	    	ship->tractoredby->vy = ship->vy;
	    	ship->tractoredby->vz = ship->vz;
	    }
	  }
	}


        if ( autofly(ship) )
           continue;
        

        for( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
	{           
/*          if ( ship->shipclass != SHIP_PLATFORM && !autofly(ship) && ship->currspeed < 50)
          {
            if ( (spaceobj->gravity > 50) && strcmp(spaceobj->name,"") && space_in_range( ship, spaceobj ) )
            {
              if (ship->vx >= spaceobj->xpos + 1 || ship->vx <= spaceobj->xpos - 1 )
                ship->vx -= URANGE(-3,(spaceobj->gravity)/(ship->vx - spaceobj->xpos)/2,3);
              if (ship->vy >= spaceobj->ypos + 1 || ship->vy <= spaceobj->ypos - 1 )
                ship->vy -= URANGE(-3,(spaceobj->gravity)/(ship->vy - spaceobj->ypos)/2,3);
              if (ship->vz >= spaceobj->zpos + 1 || ship->vz <= spaceobj->zpos - 1 )
                ship->vz -= URANGE(-3,(spaceobj->gravity)/(ship->vz - spaceobj->zpos)/2,3);
            }
          }
*/
/*
          for ( target = ship->spaceobject->first_ship; target; target = target->next_in_spaceobject)
          {
                if ( target != ship &&
                    abs(ship->vx - target->vx) < 1 &&
                    abs(ship->vy - target->vy) < 1 &&
                    abs(ship->vz - target->vz) < 1 )
                {
                    ship->collision = target->maxhull;
                    target->collision = ship->maxhull;
                }
          }
*/
          if ( spaceobj->type == SPACE_SUN && spaceobj->name && strcmp(spaceobj->name,"") &&
                     abs( (int) ( ship->vx - spaceobj->xpos )) < 10 &&
                     abs( (int) ( ship->vy - spaceobj->ypos )) < 10 &&
                     abs( (int) ( ship->vz - spaceobj->zpos )) < 10 )
                {
          /* Why the redundence of code here?
             Only need to evade once.  Darrik Vequir */
                  crashsun = 1;
                }

                if (crashsun == 1)
                {
                /* Evading the sun added by Darrik Vequir */

                  ship->hx = 10 * ship->vx;
                  ship->hy = 10 * ship->vy;
                  ship->hz = 10 * ship->vz;
                  ship->energy -= ship->currspeed/10;
                  ship->goalspeed = ship->mod->realspeed;
                  ship->accel = get_acceleration(ship);
                  echo_to_room( AT_RED , get_room_index(ship->pilotseat), "Automatic Override: Evading to avoid collision with sun!\n\r" );
    	            if ( ship->shipclass == FIGHTER_SHIP || ( ship->shipclass == MIDSIZE_SHIP && ship->mod->manuever > 50 ) )
        	           ship->shipstate = SHIP_BUSY_3;
                  else if ( ship->shipclass == MIDSIZE_SHIP || ( ship->shipclass == CAPITAL_SHIP && ship->mod->manuever > 50 ) )
        	           ship->shipstate = SHIP_BUSY_2;
                  else
        	           ship->shipstate = SHIP_BUSY;

		}

/*
                    echo_to_cockpit( AT_BLOOD+AT_BLINK, ship, "You fly directly into the sun.");
                    sprintf( buf , "%s flys directly into %s!", ship->name, ship->spaceobject->star2);
                    echo_to_system( AT_ORANGE , ship , buf , NULL );
                    destroy_ship(ship , NULL);
                    continue;
*/

        if ( ship->currspeed > 0 )
        {
          if ( spaceobj->type >= SPACE_PLANET && spaceobj->name && strcmp(spaceobj->name,"") &&
               abs( (int) ( ship->vx - spaceobj->xpos )) < 10 &&
                     abs( (int) ( ship->vy - spaceobj->ypos )) < 10 &&
                     abs( (int) ( ship->vz - spaceobj->zpos )) < 10 )
                {
                    sprintf( buf , "You begin orbitting %s.", spaceobj->name); 
                    echo_to_cockpit( AT_YELLOW, ship, buf);
                    sprintf( buf , "%s begins orbiting %s.", ship->name, spaceobj->name); 
                    echo_to_system( AT_ORANGE , ship , buf , NULL );
                    ship->inorbitof = spaceobj;
                    ship->currspeed = 0;
                }            

        }
       }
       
   }

   for ( ship = first_ship; ship; ship = ship->next )
   {
        if (ship->shipstate == SHIP_HYPERSPACE)
        {
            float tx, ty, tz;
            float dist, origdist;

            ship->hyperdistance -= ship->mod->hyperspeed;

	    dist = (float) ship->hyperdistance;
	    origdist = (float) ship->orighyperdistance;

            if ( dist == 0)
              dist = -1;

            tx = ( ship->vx - ship->jx );
            ty = ( ship->vy - ship->jy );
            tz = ( ship->vz - ship->jz );
            ship->cx = ship->vx - (tx*(dist/origdist));
	    ship->cy = ship->vy - (ty*(dist/origdist));
	    ship->cz = ship->vz - (tz*(dist/origdist));

            ship->count++;

	for( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
	  if( space_in_range_h( ship, spaceobj ) )
	  {
	    int damage;
            	    echo_to_room( AT_YELLOW, get_room_index(ship->pilotseat), "Hyperjump complete.");
            	    echo_to_ship( AT_YELLOW, ship, "The ship slams to a halt as it comes out of hyperspace.");
					// 2/18/04 - Johnson - Modified call to reflect origin of object entering the system
            	    //sprintf( buf ,"%s enters the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
					sprintf( buf ,"%s enters the starsystem from hyperspace at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
                    damage = 15* number_range( 1, 4 );
                    ship->hull -= damage;
            	    echo_to_ship( AT_YELLOW, ship, "The hull cracks from the pressure.");
		    ship->vx = ship->cx;
		    ship->vy = ship->cy;
		    ship->vz = ship->cz;
		    ship_to_spaceobject( ship, ship->currjump );
		    ship->currjump = NULL;
            	    echo_to_system( AT_YELLOW, ship, buf , NULL );
            	    ship->shipstate = SHIP_READY;
            	    STRFREE( ship->home );
            	    ship->home = STRALLOC( ship->spaceobject->name );
          }
        for( target = first_ship; target; target= target->next )
	  if (target && ship && target != ship )
	  {
	    if ( target->spaceobject && ship->spaceobject && 
	         target->shipstate != SHIP_LANDED && 
	         target->mod && target->mod->gravproj && target->mod->gravitypower &&
	         abs( (int) ( ship->vx - target->vx )) < 10*target->mod->gravproj &&
	         abs( (int) ( ship->vy - target->vy )) < 10*target->mod->gravproj &&
	         abs( (int) ( ship->vz - target->vz )) < 10*target->mod->gravproj)
	      break;
	  }
	  	
	  if( target )
	  {
            	    echo_to_room( AT_YELLOW, get_room_index(ship->pilotseat), "Hyperjump complete.");
            	    echo_to_ship( AT_YELLOW, ship, "The ship slams to a halt as it comes out of hyperspace.  An artificial gravity well surrounds you!");
					// 2/18/04 - Johnson - Modified call to reflect origin of object entering the system
            	    //sprintf( buf ,"%s enters the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
					sprintf( buf ,"%s slams into a gravity well at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
		    ship->vx = ship->cx;
		    ship->vy = ship->cy;
		    ship->vz = ship->cz;
		    ship_to_spaceobject( ship, ship->currjump );
		    ship->currjump = NULL;
            	    echo_to_system( AT_YELLOW, ship, buf , NULL );
            	    ship->shipstate = SHIP_READY;
            	    STRFREE( ship->home );
            	    ship->home = STRALLOC( ship->spaceobject->name );
          }


            if (ship->shipstate == SHIP_HYPERSPACE && ship->hyperdistance <= 0 && !ship->tracking)
            {
            	ship->count = 0;
            	ship_to_spaceobject (ship, ship->currjump);
            	
            	if (ship->spaceobject == NULL)
            	{
            	    echo_to_cockpit( AT_RED, ship, "Ship lost in Hyperspace. Make new calculations.");
            	}
            	else
            	{
            	    echo_to_room( AT_YELLOW, get_room_index(ship->pilotseat), "Hyperjump complete.");
            	    echo_to_ship( AT_YELLOW, ship, "The ship lurches slightly as it comes out of hyperspace.");
					// 2/18/04 - Johnson - Modified call to reflect origin of object entering the system
            	    //sprintf( buf ,"%s enters the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
					sprintf( buf ,"%s enters the starsystem from hyperspace at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
		    ship->cx = ship->vx;
		    ship->cy = ship->vy;
		    ship->cz = ship->vz;
		    ship_to_spaceobject( ship, ship->currjump );
		    ship->currjump = NULL;
            	    echo_to_system( AT_YELLOW, ship, buf , NULL );
            	    ship->shipstate = SHIP_READY;
            	    STRFREE( ship->home );
            	    ship->home = STRALLOC( ship->spaceobject->name );
            	}
            }
            else if ( ( ship->count >= (ship->tcount ? ship->tcount : 10 ) ) && ship->shipstate == SHIP_HYPERSPACE && ship->tracking == TRUE)
            	{
               	    ship_to_spaceobject (ship, ship->currjump);
            	if (ship->spaceobject == NULL)
            	{
            	    echo_to_cockpit( AT_RED, ship, "Ship lost in Hyperspace. Make new calculations.");
            	}
            	else
            	{

            	    echo_to_room( AT_YELLOW, get_room_index(ship->pilotseat), "Hyperjump complete.");
            	    echo_to_ship( AT_YELLOW, ship, "The ship lurches slightly as it comes out of hyperspace.");
					// 2/18/04 - Johnson - Modified call to reflect origin of object entering the system
            	    //sprintf( buf ,"%s enters the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
					sprintf( buf ,"%s enters the starsystem from hyperspace at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
		    ship->vx = ship->cx;
		    ship->vy = ship->cy;
		    ship->vz = ship->cz;
		    ship_to_spaceobject( ship, ship->currjump );
		    ship->currjump = NULL;
            	    echo_to_system( AT_YELLOW, ship, buf , NULL );
            	    ship->shipstate = SHIP_READY;
            	    STRFREE( ship->home );
            	    ship->home = STRALLOC( ship->spaceobject->name );
 		    speed = ship->mod->hyperspeed;

		    ship->jx = ship->vx + ship->tx;
		    ship->jy = ship->vy + ship->ty;
		    ship->jz = ship->vz + ship->tz;

		    for( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
		      if( space_in_range( ship, spaceobj ) )
		      {
		        ship->currjump = spaceobj;
 		        break;
		      }
		    if( !spaceobj )
		      ship->currjump = ship->spaceobject;

		    ship->hyperdistance  = abs( (int) ( ship->vx - ship->jx )) ;
    		    ship->hyperdistance += abs( (int) ( ship->vy - ship->jy )) ;
		    ship->hyperdistance += abs( (int) ( ship->vz - ship->jz )) ;
		    ship->hyperdistance /= 50;

		    ship->orighyperdistance = ship->hyperdistance;
		    
		    ship->count = 0;
		    do_radar( ship->ch, "" );
          	}
            }
            
            else if (ship->count >= 10 && ship->shipstate == SHIP_HYPERSPACE)
            {
               ship->count = 0;
               sprintf( buf ,"%d" , ship->hyperdistance );
               echo_to_room_dnr( AT_YELLOW , get_room_index(ship->pilotseat), "Remaining jump distance: " );
               echo_to_room( AT_WHITE , get_room_index(ship->pilotseat),  buf );
               
            }
            if( ship->shipstate == SHIP_HYPERSPACE )
              if( ship->count%2 && ship->hyperdistance < 10*ship->mod->hyperspeed && ship->hyperdistance > 0 )
              {
                sprintf( buf ,"An alarm sounds. Your hyperjump is ending: %d" , ship->hyperdistance );
                echo_to_ship( AT_RED , ship,  buf );
               
              }
        }
   	
     if( ship->docked )
     {
       ship->vx = ship->docked->vx;
       ship->vy = ship->docked->vy;
       ship->vz = ship->docked->vz;
       ship->cx = ship->docked->cx;
       ship->cy = ship->docked->cy;
       ship->cz = ship->docked->cz;
       ship->ox = ship->docked->ox;
       ship->oy = ship->docked->oy;
       ship->oz = ship->docked->oz;
       ship->jx = ship->docked->jx;
       ship->jy = ship->docked->jy;
       ship->jz = ship->docked->jz;
       ship->hx = ship->docked->hx;
       ship->hy = ship->docked->hy;
       ship->hz = ship->docked->hz;
       ship->shipstate = ship->docked->shipstate;
       ship->hyperdistance = ship->docked->hyperdistance;
       ship->currspeed = ship->docked->currspeed;
       ship->orighyperdistance = ship->docked->orighyperdistance;
       ship->location = ship->docked->location;
       ship->dest = ship->docked->dest;
       ship->spaceobject = ship->docked->spaceobject;
       ship->currjump = ship->docked->currjump;
     }  
     
     if( ship->vx > MAX_COORD)
       ship->vx = -MAX_COORD_S;
     if( ship->vy > MAX_COORD)
       ship->vy = -MAX_COORD_S;
     if( ship->vz > MAX_COORD)
       ship->vz = -MAX_COORD_S;
     if( ship->vx > MAX_COORD)
       ship->vx = MAX_COORD_S;
     if( ship->vy > MAX_COORD)
       ship->vy = MAX_COORD_S;
     if( ship->vz > MAX_COORD)
       ship->vz = MAX_COORD_S;
     
   }
/*
       if (ship->collision) 
       {
           echo_to_cockpit( AT_WHITE+AT_BLINK , ship,  "You have collided with another ship!" );
           echo_to_ship( AT_RED , ship , "A loud explosion shakes the ship violently!" );   
           damage_ship( ship , NULL, ship->collision , ship->collision );
           ship->collision = 0;
       }
*/
}   

void recharge_ships( )
{
   SHIP_DATA *ship;
   char buf[MAX_STRING_LENGTH];
   bool closeem = FALSE;
   int distance, origchance = 100;
   TURRET_DATA *turret;


   baycount++;

   if ( baycount >= 60 )
   {
     closeem = TRUE;
     baycount = 0;
   }

   for ( ship = first_ship; ship; ship = ship->next )
   {                

        if ( ship->shipclass == 3 )
          if ( closeem && ship->guard )
            ship->bayopen = FALSE;
           
        if (ship->statet0 > 0)
        {
           ship->energy -= ship->statet0;
           ship->statet0 = 0;
        }
        if (ship->statei0 > 0)
        {
           ship->energy -= 10*ship->statei0;
           ship->statei0 = 0;
        }

	if (ship->first_turret)
	  for( turret = ship->first_turret; turret; turret = turret->next )
	    if (turret->state > 0)
            {
              ship->energy -= turret->state;
              turret->state = 0;
            }


	if( ship->docked && ship->docked->shipclass == SHIP_PLATFORM )
	{
	  if( ship->mod->maxenergy - ship->energy > 500 )
	    ship->energy += 500;
	  else
	    ship->energy = ship->mod->maxenergy;
	}

        if (ship->missilestate == MISSILE_RELOAD_2)
        {
           ship->missilestate = MISSILE_READY;
           if ( ship->missiles > 0 )
               echo_to_room( AT_YELLOW, get_room_index(ship->gunseat), "Missile launcher reloaded.");
        }
        
        if (ship->missilestate == MISSILE_RELOAD )
        {
           ship->missilestate = MISSILE_RELOAD_2;
        }
        
        if (ship->missilestate == MISSILE_FIRED )
           ship->missilestate = MISSILE_RELOAD;
        
       if ( autofly(ship) )
       {
          if ( ship->spaceobject && ship->shipclass != SHIP_DEBRIS )
          {
             if (ship->target0 && ship->statet0 != LASER_DAMAGED )
             {
                 int chance = 75;
                 SHIP_DATA * target = ship->target0;
                 int shots, guns, laserguns, ionguns;
                 int whichguns = 0;
                 int laserhits = 0, lasermisses = 0;
                 int ionhits = 0, ionmisses = 0;
                 int laserdamage = 0, iondamage = 0;
                 int damage = 0;
                 
                 if ( ship->mod->lasers && ship->mod->ions && ship->mod->lasers < 7 && ship->mod->ions < 7 )
                   {
                     whichguns = 2;
                     guns = ship->mod->lasers + ship->mod->ions;
                     laserguns = ship->mod->lasers;
                     ionguns = ship->mod->ions;
                   }
                 else if ( ship->target0->shield > 0 && ship->mod->ions )
                 {
                   whichguns = 1;
                   guns = ship->mod->ions;
                   laserguns = 0;
                   ionguns = ship->mod->ions;
                 }
                 else
                 {
                   guns = ship->mod->lasers;
                   laserguns = ship->mod->lasers;
                   ionguns = 0;
                 }
                   
                for ( shots=0 ; shots < guns; shots++ )
                {
                  if ( !ship->target0 )
                    break;
                  if (ship->shipstate != SHIP_HYPERSPACE && ship->energy > 25 
                  && ship_in_range( ship, target )
                  && abs( (int) ( target->vx - ship->vx )) <= 1000 
                  && abs( (int) ( target->vy - ship->vy )) <= 1000
                  && abs( (int) ( target->vz - ship->vz )) <= 1000 )
                  {
                    if ( ship->shipclass > 1 || is_facing ( ship , target ) )
                    {

	             distance = abs( (int) ( target->vx - ship->vx )) 
        	       + abs( (int) ( target->vy - ship->vy )) 
	               + abs( (int) ( target->vz - ship->vz ));
		     distance /= 3;
		     chance += target->shipclass - ship->shipclass;
		     chance += ship->currspeed - target->currspeed;
		     chance += ship->mod->manuever - target->mod->manuever;
		     chance -= distance/(10*(target->shipclass+1));
		     chance -= origchance;
		     chance /= 2;
		     chance += origchance;
	             chance = URANGE( 1 , chance , 99 );

             		if ( number_percent( ) > chance )
             		{
             		  whichguns == 0 ? lasermisses++ : ( whichguns == 1 ? ionmisses++ : ( shots < ship->mod->lasers ? lasermisses++ : ionmisses++ ) );
             		} 
             		else
             		{
             		  if( whichguns == 0 )
             		  {
             		    laserhits++;
			    if ( ship->shipclass == SHIP_PLATFORM ) 
			    {      
			      damage = number_range( SHIPDAM_MINPLATFORM , SHIPDAM_MAXPLATFORM );
			    }
			    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass != CAPITAL_SHIP )
			      damage = number_range( SHIPDAM_MINCAP, SHIPDAM_MAXCAP );
			    else 
                              damage = number_range( SHIPDAM_MINDEF , SHIPDAM_MAXDEF );
             		    
             		    laserdamage += damage;
             		    
                          }
             		  else if( whichguns == 1 )
             		  {
             		    ionhits++;
			    if ( ship->shipclass == SHIP_PLATFORM ) 
			    {      
			      damage = number_range( SHIPDAM_MINPLATFORM , SHIPDAM_MAXPLATFORM );
			    }
			    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass != CAPITAL_SHIP )
			      damage = number_range( SHIPDAM_MINCAP, SHIPDAM_MAXCAP );
			    else 
                              damage = number_range( SHIPDAM_MINDEF , SHIPDAM_MAXDEF );
             		    
             		    iondamage += damage;
             		    
             		    
                          }
             		  else if( whichguns == 2 )
             		  {
             		    if( shots < ship->mod->lasers )
             		    {
             		    laserhits++;
			    if ( ship->shipclass == SHIP_PLATFORM ) 
			    {      
			      damage = number_range( SHIPDAM_MINPLATFORM , SHIPDAM_MAXPLATFORM );
			    }
			    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass != CAPITAL_SHIP )
			      damage = number_range( SHIPDAM_MINCAP, SHIPDAM_MAXCAP );
			    else 
                              damage = number_range( SHIPDAM_MINDEF , SHIPDAM_MAXDEF );
             		    
             		    laserdamage += damage;
                            }
			    else
             		    {
             		    ionhits++;
			    if ( ship->shipclass == SHIP_PLATFORM ) 
			    {      
			      damage = number_range( SHIPDAM_MINPLATFORM , SHIPDAM_MAXPLATFORM );
			    }
			    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass != CAPITAL_SHIP )
			      damage = number_range( SHIPDAM_MINCAP, SHIPDAM_MAXCAP );
			    else 
                              damage = number_range( SHIPDAM_MINDEF , SHIPDAM_MAXDEF );
             		    
             		    iondamage += damage;
                            }
                          }
                        }
                        
                    }
                  }
                }

		if ( laserhits )
		{
		  if ( lasermisses )
		  {
		    sprintf( buf, "%s fires %d lasers at you, hitting you %d times.\n\r", 
		             ship->name, laserguns, laserhits );
                    echo_to_cockpit( AT_BLOOD , target , buf );           
		    sprintf( buf, "%s fires %d lasers at %s, hitting %d times.\n\r", 
		             ship->name, laserguns, target->name, laserhits );
                    echo_to_system( AT_ORANGE , target , buf , NULL );

		  }
		  else
		  {
		    sprintf( buf, "%s fires %d lasers at you, hitting you %d times.\n\r", 
		             ship->name, laserguns, laserhits );
                    echo_to_cockpit( AT_BLOOD , target , buf );           
		    sprintf( buf, "%s fires %d lasers at %s, hitting %d times.\n\r", 
		             ship->name, laserguns, target->name, laserhits );
                    echo_to_system( AT_ORANGE , target , buf , NULL );
                  }
		  
		  damage_ship ( target, ship, laserdamage, laserdamage+1 );
		}
		else if ( lasermisses )
		{
		    sprintf( buf, "%s fires its lasers at you, missing you %d times.\n\r", 
		             ship->name, lasermisses );
                    echo_to_cockpit( AT_BLOOD , target , buf );           
		    sprintf( buf, "%s fires its lasers at %s, missing %d times.\n\r", 
		             ship->name, target->name, lasermisses );
                    echo_to_system( AT_ORANGE , target , buf , NULL );
		  
		}
		  
		if ( ionhits ) 
		{
		  if ( ionmisses )
		  {
		    sprintf( buf, "%s fires %d ion cannons at you, hitting you %d times.\n\r", 
		             ship->name, ionguns, ionhits );
                    echo_to_cockpit( AT_BLOOD , target , buf );           
		    sprintf( buf, "%s fires %d ion cannons at %s, hitting %d times.\n\r", 
		             ship->name, ionguns, target->name, ionhits );
                    echo_to_system( AT_ORANGE , target , buf , NULL );
                  }
		  else
		  {
		    sprintf( buf, "%s fires %d ion cannons at you, hitting you %d times.\n\r", 
		             ship->name, ionguns, ionhits );
                    echo_to_cockpit( AT_BLOOD , target , buf );           
		    sprintf( buf, "%s fires %d ion cannons at %s, hitting %d times.\n\r", 
		             ship->name, ionguns, target->name, ionhits );
                    echo_to_system( AT_ORANGE , target , buf , NULL );
                  }
		  
		  damage_ship ( target, ship, -1*iondamage, -1*(iondamage-1) );
		  
		}		
		else if ( ionmisses )
		{
		    sprintf( buf, "%s fires its ion cannons at you, missing you %d times.\n\r", 
		             ship->name, ionmisses );
                    echo_to_cockpit( AT_BLOOD , target , buf );           
		    sprintf( buf, "%s fires its ions cannons at %s, missing %d times.\n\r", 
		             ship->name, target->name, ionmisses );
                    echo_to_system( AT_ORANGE , target , buf , NULL );
		  
		}
		

             }
          }
       }
    
              if ( autofly(ship) )
       {
          if ( ship->spaceobject && ship->shipclass != SHIP_DEBRIS )
          {
             if (ship->target0 && ship->statet0 != LASER_DAMAGED )
             {
                 int chance = 75;
                 SHIP_DATA * target = ship->target0;
                 int shots, guns, laserguns, ionguns;
                 int whichguns = 0;
                 int laserhits = 0, lasermisses = 0;
                 int ionhits = 0, ionmisses = 0;
                 int laserdamage = 0, iondamage = 0;
                 int damage = 0;
                 
                 if ( ship->mod->lasers && ship->mod->ions && ship->mod->lasers < 7 && ship->mod->ions < 7 )
                   {
                     whichguns = 2;
                     guns = ship->mod->lasers + ship->mod->ions;
                     laserguns = ship->mod->lasers;
                     ionguns = ship->mod->ions;
                   }
                 else if ( ship->target0->shield > 0 && ship->mod->ions )
                 {
                   whichguns = 1;
                   guns = ship->mod->ions;
                   laserguns = 0;
                   ionguns = ship->mod->ions;
                 }
                 else
                 {
                   guns = ship->mod->lasers;
                   laserguns = ship->mod->lasers;
                   ionguns = 0;
                 }
                   
                for ( shots=0 ; shots < guns; shots++ )
                {
                  if ( !ship->target0 )
                    break;
                  if (ship->shipstate != SHIP_HYPERSPACE && ship->energy > 25 
                  && ship_in_range( ship, target )
                  && abs( (int) ( target->vx - ship->vx )) <= 1000 
                  && abs( (int) ( target->vy - ship->vy )) <= 1000
                  && abs( (int) ( target->vz - ship->vz )) <= 1000 )
                  {
                    if ( ship->shipclass > 1 || is_facing ( ship , target ) )
                    {

	             distance = abs( (int) ( target->vx - ship->vx )) 
        	       + abs( (int) ( target->vy - ship->vy )) 
	               + abs( (int) ( target->vz - ship->vz ));
		     distance /= 3;
		     chance += target->shipclass - ship->shipclass;
		     chance += ship->currspeed - target->currspeed;
		     chance += ship->mod->manuever - target->mod->manuever;
		     chance -= distance/(10*(target->shipclass+1));
		     chance -= origchance;
		     chance /= 2;
		     chance += origchance;
	             chance = URANGE( 1 , chance , 99 );

             		if ( number_percent( ) > chance )
             		{
             		  whichguns == 0 ? lasermisses++ : ( whichguns == 1 ? ionmisses++ : ( shots < ship->mod->lasers ? lasermisses++ : ionmisses++ ) );
             		} 
             		else
             		{
             		  if( whichguns == 0 )
             		  {
             		    laserhits++;
			    if ( ship->shipclass == SHIP_PLATFORM ) 
			    {      
			      damage = number_range( SHIPDAM_MINPLATFORM , SHIPDAM_MAXPLATFORM );
			    }
			    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass != CAPITAL_SHIP )
			      damage = number_range( SHIPDAM_MINCAP, SHIPDAM_MAXCAP );
			    else 
                              damage = number_range( SHIPDAM_MINDEF , SHIPDAM_MAXDEF );
             		    
             		    laserdamage += damage;
             		    
                          }
             		  else if( whichguns == 1 )
             		  {
             		    ionhits++;
			    if ( ship->shipclass == SHIP_PLATFORM ) 
			    {      
			      damage = number_range( SHIPDAM_MINPLATFORM , SHIPDAM_MAXPLATFORM );
			    }
			    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass != CAPITAL_SHIP )
			      damage = number_range( SHIPDAM_MINCAP, SHIPDAM_MAXCAP );
			    else 
                              damage = number_range( SHIPDAM_MINDEF , SHIPDAM_MAXDEF );
             		    
             		    iondamage += damage;
             		    
             		    
                          }
             		  else if( whichguns == 2 )
             		  {
             		    if( shots < ship->mod->lasers )
             		    {
             		    laserhits++;
			    if ( ship->shipclass == SHIP_PLATFORM ) 
			    {      
			      damage = number_range( SHIPDAM_MINPLATFORM , SHIPDAM_MAXPLATFORM );
			    }
			    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass != CAPITAL_SHIP )
			      damage = number_range( SHIPDAM_MINCAP, SHIPDAM_MAXCAP );
			    else 
                              damage = number_range( SHIPDAM_MINDEF , SHIPDAM_MAXDEF );
             		    
             		    laserdamage += damage;
                            }
			    else
             		    {
             		    ionhits++;
			    if ( ship->shipclass == SHIP_PLATFORM ) 
			    {      
			      damage = number_range( SHIPDAM_MINPLATFORM , SHIPDAM_MAXPLATFORM );
			    }
			    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass != CAPITAL_SHIP )
			      damage = number_range( SHIPDAM_MINCAP, SHIPDAM_MAXCAP );
			    else 
                              damage = number_range( SHIPDAM_MINDEF , SHIPDAM_MAXDEF );
             		    
             		    iondamage += damage;
                            }
                          }
                        }
                        
                    }
                  }
                }

		if ( laserhits )
		{
		  if ( lasermisses )
		    sprintf( buf, "%s fires %d lasers at you, hitting you %d times and missing you %d times.\n\r", 
		             ship->name, laserguns, laserhits, lasermisses );
		  else
		    sprintf( buf, "%s fires %d lasers at you, hitting you %d times.\n\r", 
		             ship->name, laserguns, laserhits );
		  
		  damage_ship ( target, ship, laserdamage, laserdamage+1 );
		}
		else if ( lasermisses )
		{
		    sprintf( buf, "%s fires its lasers at you, missing you %d times.\n\r", 
		             ship->name, lasermisses );
		  
		}
		  
		if ( ionhits ) 
		{
		  if ( ionmisses )
		    sprintf( buf, "%s fires %d ion cannons at you, hitting you %d times and missing you %d times.\n\r", 
		             ship->name, ionguns, ionhits, ionmisses );
		  else
		    sprintf( buf, "%s fires %d ion cannons at you, hitting you %d times.\n\r", 
		             ship->name, ionguns, ionhits );
		  
		  damage_ship ( target, ship, -1*iondamage, -1*(iondamage-1) );
		  
		}		
		else if ( ionmisses )
		{
		    sprintf( buf, "%s fires its ion cannons at you, missing you %d times.\n\r", 
		             ship->name, ionmisses );
		  
		}
		

             }
          }
       }

   }
}

void update_space( )
{
   SHIP_DATA *ship;
   SHIP_DATA *target;
   char buf[MAX_STRING_LENGTH];
   int too_close, target_too_close;
   SPACE_DATA *spaceobj;
   TURRET_DATA *turret;
   int recharge;

   for ( ship = first_ship; ship; ship = ship->next )
   {
          if ( ship->spaceobject && ship->energy > 0 && ship->shipstate == SHIP_DISABLED && ship->shipclass != SHIP_PLATFORM )
             ship->energy -= 100;
          else if (ship->energy > 0 && ( !ship->mod || !ship->mod->gravproj ) ) // Energy does not recharge when gravity wells are activated  - DV 8/7/02
            ship->energy += (5 + ship->shipclass * 5);
          else if (ship->mod && ship->mod->gravproj ) // Energy drains 1/100th of gravity power every space pulse - DV 8/7/02
            ship->energy -= ship->mod->gravproj/100;
          else
             ship->energy = 0;

        if ( ship->chaff_released > 0 )
           ship->chaff_released--;
        
        /* following was originaly to fix ships that lost their pilot 
           in the middle of a manuever and are stuck in a busy state 
           but now used for timed manouevers such as turning */
    
        if( ship->shipstate == SHIP_READY && ship->tracking == TRUE )
	{
         if( ship->count == 0 )
         {
           ship->count++;
         }
         else
         {
          do_hyperspace( ship->ch, "" );
          ship->count = 0;
         }
        }         
    
    	if (ship->shipstate == SHIP_BUSY_3)
           {
              echo_to_room( AT_YELLOW, get_room_index(ship->pilotseat), "Manuever complete.");
              ship->shipstate = SHIP_READY;
           }
        if (ship->shipstate == SHIP_BUSY_2)
           ship->shipstate = SHIP_BUSY_3;
        if (ship->shipstate == SHIP_BUSY)
           ship->shipstate = SHIP_BUSY_2;
        
        if (ship->shipstate == SHIP_LAND_2)
           landship( ship , ship->dest );
        if (ship->shipstate == SHIP_LAND)
        {
           approachland( ship, ship->dest );
           ship->shipstate = SHIP_LAND_2;
        }
        
        if (ship->shipstate == SHIP_LAUNCH_2)
           launchship( ship );
        if (ship->shipstate == SHIP_LAUNCH)
           ship->shipstate = SHIP_LAUNCH_2;

        if (ship->docking == SHIP_DOCK_2)
           dockship( ship->ch , ship );
        if (ship->docking == SHIP_DOCK)
           ship->docking = SHIP_DOCK_2;
        

        ship->shield = UMAX( 0 , ship->shield-1-ship->shipclass);
                
        if (ship->autorecharge && ship->mod->maxshield > ship->shield && ship->energy > 100)
        {
           recharge  = UMIN( ship->mod->maxshield-ship->shield, 10 + ship->shipclass*10 );           
           recharge  = UMIN( recharge , ship->energy/2 -100 );
           recharge  = UMAX( 1, recharge );
           ship->shield += recharge;
           ship->energy -= recharge;        
        }
        
        if ( autofly(ship) && ( ship->energy >= ((25+ship->shipclass*25)*(2+ship->shipclass) ) )
        && ((ship->mod->maxshield - ship->shield) >= ( 25+ship->shipclass*25 ) ) )
        {
    	  recharge  = 25+ship->shipclass*25;
	  recharge  = UMIN(  ship->mod->maxshield-ship->shield , recharge );
    	  ship->shield += recharge;
    	  ship->energy -= ( recharge*2 + recharge * ship->shipclass );        
    	}
        
        
        if (ship->shield > 0)
        {
          if (ship->energy < 200)
          {
          	ship->shield = 0;
          	echo_to_cockpit( AT_RED, ship,"The ships shields fizzle and die.");
                ship->autorecharge = FALSE;
          }
        }        
        
        if ( ship->spaceobject && ship->currspeed > 0 )
        {
               sprintf( buf, "%d",
                          ship->currspeed );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->pilotseat),  "Speed: " );
               echo_to_room_dnr( AT_LBLUE , get_room_index(ship->pilotseat),  buf );
               sprintf( buf, "%.0f %.0f %.0f",
                           ship->vx , ship->vy, ship->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->pilotseat),  "  Coords: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->pilotseat),  buf );
            if ( ship->pilotseat != ship->coseat )
            {
               sprintf( buf, "%d",
                          ship->currspeed );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->coseat),  "Speed: " );
               echo_to_room_dnr( AT_LBLUE , get_room_index(ship->coseat),  buf );
               sprintf( buf, "%.0f %.0f %.0f",
                           ship->vx , ship->vy, ship->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->coseat),  "  Coords: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->coseat),  buf );
            }
        } 

        if ( ship->spaceobject )
        {
          too_close = ship->currspeed + 50;
          too_close = ship->currspeed + 50;

	 for( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
          if ( spaceobj->name &&  strcmp(spaceobj->name,"") && 
               abs( (int) ( ship->vx - spaceobj->xpos )) < too_close && 
               abs( (int) ( ship->vy - spaceobj->ypos )) < too_close &&
               abs( (int) ( ship->vz - spaceobj->zpos )) < too_close )
          {
                sprintf( buf, "Proximity alert: %s  %.0f %.0f %.0f", spaceobj->name,
                         spaceobj->xpos, spaceobj->ypos, spaceobj->zpos);
                echo_to_room( AT_RED , get_room_index(ship->pilotseat),  buf );
          }

          for ( target = first_ship; target; target = target->next )
          { 
            if( (target->docked && target->docked == ship) || (ship->docked &&  ship->docked == target ) )
              continue;
               if ( ship->docked && target->docked && 
                    target->docked == ship->docked )
                 continue;
          	
            target_too_close = too_close+target->currspeed;
            if( target->spaceobject )
              if ( target != ship &&
                 abs( (int) ( ship->vx - target->vx )) < target_too_close &&
                 abs( (int) ( ship->vy - target->vy )) < target_too_close &&
                 abs( (int) ( ship->vz - target->vz )) < target_too_close &&
                 ship->docked != target && target->docked != ship )
              {
                 sprintf( buf, "Proximity alert: %s  %.0f %.0f %.0f"
                            , target->name, target->vx - ship->vx, target->vy - ship->vy, target->vz - ship->vz);
                 echo_to_room( AT_RED , get_room_index(ship->pilotseat),  buf );    
              }                
          }
          too_close = ship->currspeed + 100;
        }
       

        if (ship->target0 && ship->shipclass <= SHIP_PLATFORM)
        {
               sprintf( buf, "%s   %.0f %.0f %.0f", ship->target0->name,
                          ship->target0->vx , ship->target0->vy, ship->target0->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->gunseat), "Target: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->gunseat),  buf );
               if (!ship_in_range( ship, ship->target0 ) )
               {
	                echo_to_room( AT_LBLUE , get_room_index(ship->gunseat),  "Your target seems to have left.");
               		ship->target0 = NULL;
               }
         }
         
         if ( ship->first_turret )
           for( turret = ship->first_turret; turret; turret = turret->next )
             if( turret->target && ship->shipclass <= SHIP_PLATFORM )
             {
               sprintf( buf, "%s   %.0f %.0f %.0f", turret->target->name,
                          turret->target->vx , turret->target->vy, turret->target->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(turret->roomvnum), "Target: " );
               echo_to_room( AT_LBLUE , get_room_index(turret->roomvnum),  buf );
               if (!ship_in_range( ship, turret->target ) )
               {
               		turret->target = NULL;
	                echo_to_room( AT_LBLUE , get_room_index(turret->roomvnum),  "Your target seems to have left.");
	       }
             }
             	
/*        if (ship->target1 && ship->shipclass <= SHIP_PLATFORM)
        {
               sprintf( buf, "%s   %.0f %.0f %.0f", ship->target1->name,
                          ship->target1->vx , ship->target1->vy, ship->target1->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->turret1), "Target: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->turret1),  buf );
               if (!ship_in_range( ship, ship->target1 ) )
               		ship->target1 = NULL;
         }
        
        if (ship->target2 && ship->shipclass <= SHIP_PLATFORM)
        {
               sprintf( buf, "%s   %.0f %.0f %.0f", ship->target2->name,
                          ship->target2->vx , ship->target2->vy, ship->target2->vz );
               echo_to_room_dnr( AT_BLUE , get_room_index(ship->turret2), "Target: " );
               echo_to_room( AT_LBLUE , get_room_index(ship->turret2),  buf );
               if (!ship_in_range( ship, ship->target2 ) )
               		ship->target2 = NULL;
         }
*/
         
   	if (ship->energy < 100 && ship->spaceobject )
   	{
   	    echo_to_cockpit( AT_RED , ship,  "Warning: Ship fuel low." );
        }
                    
        ship->energy = URANGE( 0 , ship->energy, ship->mod->maxenergy );
   } 

   for ( ship = first_ship; ship; ship = ship->next )
   {
   	  if( ship->target0 && autofly(ship) )
       	   if( !ship_in_range( ship->target0, ship ) )
       	   {
             echo_to_room( AT_BLUE , get_room_index(ship->pilotseat), "Target left, returning to NORMAL condition.\n\r" );
             ship->goalspeed = 0;
             ship->accel = get_acceleration( ship );
       	     ship->target0 = NULL;
       	   }


       if (ship->autotrack && ship->docking == SHIP_READY && ship->target0 && ship->shipclass < 3 )
       {
       	     
           target = ship->target0;
           too_close = ship->currspeed + 10;
           target_too_close = too_close+target->currspeed;
           if ( target != ship && ship->shipstate == SHIP_READY &&
           	 ship->docked == NULL && ship->shipstate != SHIP_DOCKED &&
                 abs( (int) ( ship->vx - target->vx )) < target_too_close &&
                 abs( (int) ( ship->vy - target->vy )) < target_too_close &&
                 abs( (int) ( ship->vz - target->vz )) < target_too_close )
           {
              ship->hx = 0-(ship->target0->vx - ship->vx);
              ship->hy = 0-(ship->target0->vy - ship->vy);
              ship->hz = 0-(ship->target0->vz - ship->vz);
              ship->energy -= ship->currspeed/10;
              echo_to_room( AT_RED , get_room_index(ship->pilotseat), "Autotrack: Evading to avoid collision!\n\r" );
    	      if ( ship->shipclass == FIGHTER_SHIP || ( ship->shipclass == MIDSIZE_SHIP && ship->mod->manuever > 50 ) )
        	ship->shipstate = SHIP_BUSY_3;
              else if ( ship->shipclass == MIDSIZE_SHIP || ( ship->shipclass == CAPITAL_SHIP && ship->mod->manuever > 50 ) )
        	ship->shipstate = SHIP_BUSY_2;
    	      else
        	ship->shipstate = SHIP_BUSY;
           }
           else if  ( !is_facing(ship, ship->target0) 
             && ship->docked == NULL && ship->shipstate != SHIP_DOCKED )
           {
              ship->hx = ship->target0->vx - ship->vx;
              ship->hy = ship->target0->vy - ship->vy;
              ship->hz = ship->target0->vz - ship->vz;
              ship->energy -= ship->currspeed/10;
              echo_to_room( AT_BLUE , get_room_index(ship->pilotseat), "Autotracking target ... setting new course.\n\r" );
    	      if ( ship->shipclass == FIGHTER_SHIP || ( ship->shipclass == MIDSIZE_SHIP && ship->mod->manuever > 50 ) )
        	ship->shipstate = SHIP_BUSY_3;
              else if ( ship->shipclass == MIDSIZE_SHIP || ( ship->shipclass == CAPITAL_SHIP && ship->mod->manuever > 50 ) )
        	ship->shipstate = SHIP_BUSY_2;
    	      else
        	ship->shipstate = SHIP_BUSY;     
           }
       }

       if ( autofly(ship) && ship->shipclass != SHIP_DEBRIS )
       {
          if ( ship->spaceobject )
          {
             check_hostile( ship );
             if (ship->target0 )
             {
                 int chance = 50;
		 int projectiles = -1;		

		 if ( !ship->target0->target0 && autofly(ship->target0))
		   ship->target0->target0 = ship;

                 /* auto assist ships */
                  
                 for ( target = first_ship; target; target = target->next)
                 {
                   if( ship_in_range( ship, target ) )
                    if ( autofly(target) && target->docked == NULL && target->shipstate != SHIP_DOCKED )
                       if ( !str_cmp ( target->owner , ship->owner ) && target != ship )
                         if ( target->target0 == NULL && ship->target0 != target )
                         {  
                           target->target0 = ship->target0;
                           sprintf( buf , "You are being targetted by %s." , target->name);  
                           echo_to_cockpit( AT_BLOOD , target->target0 , buf );
                           break;
                        }   
                 }
                   
                 target = ship->target0;
                 ship->autotrack = TRUE;
                 if( ship->shipclass != SHIP_PLATFORM && !ship->guard 
                     && ship->docked == NULL && ship->shipstate != SHIP_DOCKED )
                 {
                      ship->goalspeed = ship->mod->realspeed;
                      ship->accel = get_acceleration( ship );
                 }
                 if ( ship->energy >200  )
                    ship->autorecharge=TRUE;
                    

                 if (ship->shipstate != SHIP_HYPERSPACE && ship->energy > 25
                 && ship->missilestate == MISSILE_READY && ship_in_range( ship, target )
                 && abs( (int) ( target->vx - ship->vx )) <= 1200
                 && abs( (int) ( target->vy - ship->vy )) <= 1200
                 && abs( (int) ( target->vz - ship->vz )) <= 1200 )
                 {
                   if ( ship->shipclass > 1 || is_facing( ship , target ) )
                   {
             		chance -= target->mod->manuever/5;
                        chance -= target->currspeed/20;
                        chance += target->shipclass*target->shipclass*25;
                        chance -= ( abs( (int) ( target->vx - ship->vx ))/100 );
                        chance -= ( abs( (int) ( target->vy - ship->vy ))/100 );
                        chance -= ( abs( (int) ( target->vz - ship->vz ))/100 );
                        chance += ( 30 );
                        chance = URANGE( 10 , chance , 90 );

		   if( ( target->shipclass == SHIP_PLATFORM || ( target->shipclass == CAPITAL_SHIP && target->currspeed < 50 )) && ship->rockets > 0 )
		     projectiles = HEAVY_ROCKET;
		   else if ( ( target->shipclass == MIDSIZE_SHIP || ( target->shipclass == CAPITAL_SHIP) ) && ship->torpedos > 0 )
		     projectiles = PROTON_TORPEDO;
		   else if ( ship->missiles < 0 && ship->torpedos > 0 )
		     projectiles = PROTON_TORPEDO;
		   else if ( ship->missiles < 0 && ship->rockets > 0 )
		     projectiles = HEAVY_ROCKET;
		   else if ( ship->missiles > 0 )
		     projectiles = CONCUSSION_MISSILE;
		   else
		     projectiles = -1;
		     
             		if ( number_percent( ) > chance || projectiles == -1 )
             		{
             		}
                        else
                        {
                                new_missile( ship , target , NULL , projectiles );
            		 	if( projectiles == CONCUSSION_MISSILE ) ship->missiles--;
            		 	if( projectiles == PROTON_TORPEDO ) ship->torpedos--;
            		 	if( projectiles == HEAVY_ROCKET ) ship->rockets--;
             		        sprintf( buf , "Incoming projectile from %s." , ship->name);
             		        echo_to_cockpit( AT_BLOOD , target , buf );
             		        sprintf( buf, "%s fires a projectile towards %s." , ship->name, target->name );
             		        echo_to_system( AT_ORANGE , target , buf , NULL );

            		 	if ( ship->shipclass == CAPITAL_SHIP || ship->shipclass == SHIP_PLATFORM )
                                    ship->missilestate = MISSILE_RELOAD_2;
                                else
                                    ship->missilestate = MISSILE_FIRED;
                        }
                   }
                 }

                 if( ship->missilestate ==  MISSILE_DAMAGED )
                     ship->missilestate =  MISSILE_READY;
                 if( ship->statet0 ==  LASER_DAMAGED )
                     ship->statet0 =  LASER_READY;
                 if( ship->statei0 ==  LASER_DAMAGED )
                     ship->statei0 =  LASER_READY;
                 if( ship->shipstate ==  SHIP_DISABLED )
                     ship->shipstate =  SHIP_READY;

             }
             else
               if( ship->currspeed )
               {
                 ship->goalspeed = 0;
                 ship->accel = get_acceleration( ship );
               }
          }
          else
          {
               if ( number_range(1, 25) == 25 )
               {
          	  ship_to_spaceobject(ship, spaceobject_from_name(ship->home) );
		  if( ship->spaceobject )
		  {
          	    ship->vx = ship->spaceobject->xpos + number_range( -5000 , 5000 );
          	    ship->vy = ship->spaceobject->ypos + number_range( -5000 , 5000 );
          	    ship->vz = ship->spaceobject->zpos + number_range( -5000 , 5000 );
                    ship->hx = 1;
                    ship->hy = 1;
                    ship->hz = 1;
                  }
               }
          }
       }

       if ( ( ship->shipclass == CAPITAL_SHIP || ship->shipclass == SHIP_PLATFORM )
       && ship->target0 == NULL )
       {
       	 int ammo = ship->missiles + ship->torpedos*2 + ship->rockets*3;
       	 
          if( ammo < ship->mod->launchers * 8 )
             ship->missiles++;
          if( ammo < ship->mod->launchers * 8 * 2 )
             ship->torpedos++;
          if( ammo < ship->mod->launchers * 8 * 3 )
             ship->rockets++;
          if( ship->chaff < ship->mod->defenselaunchers * 6 )
             ship->chaff++;
       }
       
       save_ship( ship );
   }

}



void write_spaceobject_list( )
{
    SPACE_DATA *tspaceobject;
    FILE *fpout;
    char filename[256];

    sprintf( filename, "%s%s", SPACE_DIR, SPACE_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
         bug( "FATAL: cannot open space.lst for writing!\n\r", 0 );
         return;
    }
    for ( tspaceobject = first_spaceobject; tspaceobject; tspaceobject = tspaceobject->next )
      fprintf( fpout, "%s\n", tspaceobject->filename );
    fprintf( fpout, "$\n" );
    fclose( fpout );
}


/*
 * Get pointer to space structure from spaceobject name.
 */
SPACE_DATA *spaceobject_from_name( char *name )
{
    SPACE_DATA *spaceobject;
    
    for ( spaceobject = first_spaceobject; spaceobject; spaceobject = spaceobject->next )
       if ( !str_cmp( name, spaceobject->name ) )
         return spaceobject;
    
    for ( spaceobject = first_spaceobject; spaceobject; spaceobject = spaceobject->next )
       if ( !str_prefix( name, spaceobject->name ) )
         return spaceobject;
    
    return NULL;
}

/*
 * Get pointer to space structure from the dock vnun.
 */
SPACE_DATA *spaceobject_from_vnum( int vnum )
{
    SPACE_DATA *spaceobject;
    SHIP_DATA *ship;

    for ( spaceobject = first_spaceobject; spaceobject; spaceobject = spaceobject->next )
       if ( vnum == spaceobject->doca ||
            vnum == spaceobject->docb ||
            vnum == spaceobject->docc )
         return spaceobject;

    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->hanger )
       {
         if( !(ship->bayopen) )
           return NULL; 
         return ship->spaceobject;
        }

    return NULL;
}


/*
 * Save a spaceobject's data to its data file
 */
void save_spaceobject( SPACE_DATA *spaceobject )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];
    CARGO_DATA_LIST *cargolist;

    if ( !spaceobject )
    {
	bug( "save_spaceobject: null pointer!", 0 );
	return;
    }

    if ( !spaceobject->filename || spaceobject->filename[0] == '\0' )
    {
	sprintf( buf, "save_spaceobject: %s has no filename", spaceobject->name );
	bug( buf, 0 );
	return;
    }
 
    sprintf( filename, "%s%s", SPACE_DIR, spaceobject->filename );
    
    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_spaceobject: fopen", 0 );
    	perror( filename );
    }
    else
    {
	fprintf( fp, "#SPACE\n" );
	fprintf( fp, "Name         %s~\n",	spaceobject->name	);
	fprintf( fp, "Filename     %s~\n",	spaceobject->filename	);
	fprintf( fp, "Type         %d\n",	spaceobject->type       );
	fprintf( fp, "Locationa      %s~\n",	spaceobject->locationa	);
	fprintf( fp, "Locationb      %s~\n",	spaceobject->locationb	);
	fprintf( fp, "Locationc      %s~\n",	spaceobject->locationc	);
	fprintf( fp, "Doca          %d\n",	spaceobject->doca	);
	fprintf( fp, "Docb          %d\n",      spaceobject->docb       );
	fprintf( fp, "Docc          %d\n",      spaceobject->docc       );        	
	fprintf( fp, "Seca          %d\n",	spaceobject->seca	);
	fprintf( fp, "Secb          %d\n",      spaceobject->secb       );
	fprintf( fp, "Secc          %d\n",      spaceobject->secc       );        	
	fprintf( fp, "Gravity     %d\n",       spaceobject->gravity    );
	fprintf( fp, "Xpos          %.0f\n",       spaceobject->xpos    );
	fprintf( fp, "Ypos          %.0f\n",       spaceobject->ypos    );	
	fprintf( fp, "Zpos          %.0f\n",       spaceobject->zpos    );	
	fprintf( fp, "HX            %.0f\n",       spaceobject->hx      );
	fprintf( fp, "HY            %.0f\n",       spaceobject->hy      );	
	fprintf( fp, "HZ            %.0f\n",       spaceobject->hz      );	
	fprintf( fp, "SP            %d\n",       spaceobject->speed   );	
	fprintf( fp, "Trainer       %d\n",       spaceobject->trainer    );	
        if ( spaceobject->first_cargo )
        {
	  for( cargolist = spaceobject->first_cargo; cargolist; cargolist = cargolist->next )
	  {
	    fprintf( fp, "Cargo %d %d\n", 
	       cargolist->cargo->cargotype, cargolist->cargo->price );	
	  }
        }
	fprintf( fp, "End\n\n"						);

	fprintf( fp, "#END\n"						);
    }
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Read in actual spaceobject data.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

void fread_spaceobject( SPACE_DATA *spaceobject, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    const char *word;
    bool fMatch;
    CARGO_DATA_LIST *cargolist;
    CARGO_DATA *cargo;

 
    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;

	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;

        case 'C':
	    if ( !str_cmp( word, "Cargo" ) )
	    {
		int x1,x2;
		char *ln = fread_line( fp );

		x1=x2=0;
		sscanf( ln, "%d %d", &x1, &x2 );

		CREATE( cargolist, CARGO_DATA_LIST, 1 );
  
  		LINK( cargolist, spaceobject->first_cargo, spaceobject->last_cargo, next, prev );
  
  		CREATE( cargo, CARGO_DATA, 1 );

		cargolist->cargo = cargo;
  
		cargolist->cargo->cargotype	= x1;
		cargolist->cargo->price	= x2;
		fMatch		= TRUE;
		break;
	    }

        case 'D':
             KEY( "Doca",  spaceobject->doca,          fread_number( fp ) );
             KEY( "Docb",  spaceobject->docb,          fread_number( fp ) );
             KEY( "Docc",  spaceobject->docc,          fread_number( fp ) );
             break;
                                

	case 'E':
	    if ( !str_cmp( word, "End" ) )
	    {
		if (!spaceobject->name)
		  spaceobject->name		= STRALLOC( "" );
		if (!spaceobject->locationa)
		  spaceobject->locationa            = STRALLOC( "" );  
		if (!spaceobject->locationb)
		  spaceobject->locationb            = STRALLOC( "" );  
		if (!spaceobject->locationc)
		  spaceobject->locationc            = STRALLOC( "" );  
		return;
	    }
	    break;
	    
	case 'F':
	    KEY( "Filename",	spaceobject->filename,		fread_string_nohash( fp ) );
	    break;
        
        case 'G':
            KEY( "Gravity",  spaceobject->gravity,     fread_number( fp ) ); 
            break;  
        
        case 'H':
            KEY( "HX",  spaceobject->hx,     fread_number( fp ) ); 
            KEY( "HY",  spaceobject->hy,     fread_number( fp ) ); 
            KEY( "HZ",  spaceobject->hz,     fread_number( fp ) ); 
            break;  

        case 'L':
	    KEY( "Locationa",	spaceobject->locationa,		fread_string( fp ) );
	    KEY( "Locationb",	spaceobject->locationb,		fread_string( fp ) );
	    KEY( "Locationc",	spaceobject->locationc,		fread_string( fp ) );
	    break;
  
	case 'N':
	    KEY( "Name",	spaceobject->name,		fread_string( fp ) );
	    break;
/*        
        case 'P':
             break;

       	     KEY( "Star1",	spaceobject->star1,	fread_string( fp ) );
	     KEY( "Star2",	spaceobject->star2,	fread_string( fp ) );
	     KEY( "S1x",  spaceobject->s1x,          fread_number( fp ) ); 
             KEY( "S1y",  spaceobject->s1y,          fread_number( fp ) ); 
             KEY( "S1z",  spaceobject->s1z,          fread_number( fp ) ); 
             KEY( "S2x",  spaceobject->s2x,          fread_number( fp ) ); 
             KEY( "S2y",  spaceobject->s2y,          fread_number( fp ) );
             KEY( "S2z",  spaceobject->s2z,          fread_number( fp ) );
*/
       	case 'S':
            KEY( "Seca", spaceobject->seca, 		fread_number( fp ) );
            KEY( "Secb", spaceobject->secb, 		fread_number( fp ) );
            KEY( "Secc", spaceobject->secc, 		fread_number( fp ) );
            KEY( "SP", spaceobject->speed, 		fread_number( fp ) );
        case 'T':
            KEY( "Trainer",  spaceobject->trainer,     fread_number( fp ) ); 
            KEY( "Type",  spaceobject->type,	       fread_number( fp ) ); 
            
        case 'X':
            KEY( "Xpos",  spaceobject->xpos,     fread_number( fp ) ); 
        
        case 'Y':
            KEY( "Ypos",  spaceobject->ypos,     fread_number( fp ) ); 

        case 'Z':
            KEY( "Zpos",  spaceobject->zpos,     fread_number( fp ) ); 
                 
       	}

	if ( !fMatch )
	{
	    sprintf( buf, "Fread_spaceobject: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

/*
 * Load a spaceobject file
 */

bool load_spaceobject( char *spaceobjectfile )
{
    char filename[256];
    SPACE_DATA *spaceobject;
    FILE *fp;
    bool found;

    CREATE( spaceobject, SPACE_DATA, 1 );

    found = FALSE;
    sprintf( filename, "%s%s", SPACE_DIR, spaceobjectfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
        LINK( spaceobject, first_spaceobject, last_spaceobject, next, prev );
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_spaceobject_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "SPACE"	) )
	    {
	    	fread_spaceobject( spaceobject, fp );
	    	break;
	    }
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "Load_spaceobject_file: bad section: %s.", word );
		bug( buf, 0 );
		break;
	    }
	}
	fclose( fp );
    }

    if ( !(found) )
      DISPOSE( spaceobject );

    return found;
}

/*
 * Load in all the spaceobject files.
 */
void load_space( )
{
    FILE *fpList;
    const char *filename;
    char spaceobjectlist[256];
    char buf[MAX_STRING_LENGTH];
    
    
    first_spaceobject	= NULL;
    last_spaceobject	= NULL;

    log_string( "Loading space..." );

    sprintf( spaceobjectlist, "%s%s", SPACE_DIR, SPACE_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( spaceobjectlist, "r" ) ) == NULL )
    {
	perror( spaceobjectlist );
	exit( 1 );
    }

    for ( ; ; )
    {
	filename = feof( fpList ) ? "$" : fread_word( fpList );
	if ( filename[0] == '$' )
	  break;
	  
       
	if ( !load_spaceobject( (char*) filename ) )
	{
	  sprintf( buf, "Cannot load spaceobject file: %s", filename );
	  bug( buf, 0 );
	}
    }
    fclose( fpList );
    log_string(" Done spaceobjects " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void do_setspaceobject( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    SPACE_DATA *spaceobject;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg2[0] == '\0' || arg1[0] == '\0' )
    {
	send_to_char( "Usage: setspaceobject <spaceobject> <field> <values>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( "name filename type trainer,\n\r", ch );
	send_to_char( "xpos ypos zpos gravity seca secb secc,\n\r", ch );
	send_to_char( "locationa locationb locationc doca docb docc\n\r", ch );
	send_to_char( "", ch );
	return;
    }

    spaceobject = spaceobject_from_name( arg1 );
    if ( !spaceobject )
    {
	send_to_char( "No such spaceobject.\n\r", ch );
	return;
    }


    if ( !str_cmp( arg2, "trainer" ) )
    {
      if ( spaceobject->trainer )
	spaceobject->trainer = 0;
      else
        spaceobject->trainer = 1;
        
      send_to_char( "Done.\n\r", ch );
      save_spaceobject( spaceobject );
      return;
    }
    if ( !str_cmp( arg2, "seca" ) )
    {
      if ( spaceobject->seca )
	spaceobject->seca = 0;
      else
        spaceobject->seca = 1;
        
      send_to_char( "Done.\n\r", ch );
      save_spaceobject( spaceobject );
      return;
    }
    if ( !str_cmp( arg2, "secb" ) )
    {
      if ( spaceobject->secb )
	spaceobject->secb = 0;
      else
        spaceobject->secb = 1;
        
      send_to_char( "Done.\n\r", ch );
      save_spaceobject( spaceobject );
      return;
    }
    if ( !str_cmp( arg2, "secc" ) )
    {
      if ( spaceobject->secc )
	spaceobject->secc = 0;
      else
        spaceobject->secc = 1;
        
      send_to_char( "Done.\n\r", ch );
      save_spaceobject( spaceobject );
      return;
    }
    if ( !str_cmp( arg2, "type" ) )
    {
	spaceobject->type = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }

    if ( !str_cmp( arg2, "doca" ) )
    {
	spaceobject->doca = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    if ( !str_cmp( arg2, "docb" ) )
    {
	spaceobject->docb = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    if ( !str_cmp( arg2, "docc" ) )
    {
	spaceobject->docc = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }


    if ( !str_cmp( arg2, "xpos" ) )
    {
	spaceobject->xpos = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }

    if ( !str_cmp( arg2, "ypos" ) )
    {
	spaceobject->ypos = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    
    if ( !str_cmp( arg2, "zpos" ) )
    {
	spaceobject->zpos = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    
    if ( !str_cmp( arg2, "gravity" ) )
    {
	spaceobject->gravity = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    if ( !str_cmp( arg2, "hx" ) )
    {
	spaceobject->hx = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }

    if ( !str_cmp( arg2, "hy" ) )
    {
	spaceobject->hy = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    
    if ( !str_cmp( arg2, "hz" ) )
    {
	spaceobject->hz = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    
    if ( !str_cmp( arg2, "speed" ) )
    {
	spaceobject->speed = atoi( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    
    if ( !str_cmp( arg2, "name" ) )
    {
	STRFREE( spaceobject->name );
	spaceobject->name = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }

    if ( !str_cmp( arg2, "filename" ) )
    {
	DISPOSE( spaceobject->filename );
	spaceobject->filename = strdup( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    
    if ( !str_cmp( arg2, "locationa" ) )
    {
	STRFREE( spaceobject->locationa );
	spaceobject->locationa = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    if ( !str_cmp( arg2, "locationb" ) )
    {
	STRFREE( spaceobject->locationb );
	spaceobject->locationb = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }
    if ( !str_cmp( arg2, "locationc" ) )
    {
	STRFREE( spaceobject->locationc );
	spaceobject->locationc = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_spaceobject( spaceobject );
	return;
    }

    do_setspaceobject( ch, "" );
    return;
}

void showspaceobject( CHAR_DATA *ch , SPACE_DATA *spaceobject )
{   
    ch_printf( ch, "Space object:%s     Trainer: %s	Filename: %s  Type %d\n\r",
    			spaceobject->name, 
    			(spaceobject->trainer ? "Yes" : "No"),
    			spaceobject->filename, spaceobject->type );
    ch_printf( ch, "Coordinates: %.0f %0.f %.0f\n\rGravity: %d\n\r",
    			spaceobject->xpos , spaceobject->ypos, spaceobject->zpos, spaceobject->gravity);
    ch_printf( ch, "     doca: %5d (%s)\n\r",
    			spaceobject->doca, spaceobject->locationa);
    ch_printf( ch, "     docb: %5d (%s)\n\r",
    			spaceobject->docb, spaceobject->locationb);
    ch_printf( ch, "     docc: %5d (%s)\n\r",
    			spaceobject->docc, spaceobject->locationc);
    return;
}

void do_showspaceobject( CHAR_DATA *ch, char *argument )
{
   SPACE_DATA *spaceobject;

   spaceobject = spaceobject_from_name( argument );
   
   if ( spaceobject == NULL )
      send_to_char("&RNo such spaceobject.\n\r",ch);
   else
      showspaceobject(ch , spaceobject);
   
}

void do_makespaceobject( CHAR_DATA *ch, char *argument )
{   
    char arg[MAX_INPUT_LENGTH];
    char filename[256];
    SPACE_DATA *spaceobject;

    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makespaceobject <spaceobject name>\n\r", ch );
	return;
    }


    CREATE( spaceobject, SPACE_DATA, 1 );
    LINK( spaceobject, first_spaceobject, last_spaceobject, next, prev );

    spaceobject->name		= STRALLOC( argument );
    
		  spaceobject->locationa            = STRALLOC( "" );  
		  spaceobject->locationb            = STRALLOC( "" );  
		  spaceobject->locationc            = STRALLOC( "" );  
    
    argument = one_argument( argument, arg );
    sprintf( filename, "%s.system" , strlower(arg) );
    spaceobject->filename = str_dup( filename );
    save_spaceobject( spaceobject );
    write_spaceobject_list();
}

void do_spaceobjects( CHAR_DATA *ch, char *argument )
{
    SPACE_DATA *spaceobject;
    int count = 0;

    set_char_color( AT_RED, ch );

    for ( spaceobject = first_spaceobject; spaceobject; spaceobject = spaceobject->next )
    {
        if( spaceobject->type > SPACE_SUN )
          continue;
        if ( !(spaceobject->trainer && (!IS_GOD(ch))) )
          ch_printf( ch, "%s\n\r", spaceobject->name );
        count++;
    }

    ch_printf( ch, "\n\r" );

    set_char_color( AT_NOTE, ch );

    for ( spaceobject = first_spaceobject; spaceobject; spaceobject = spaceobject->next )
    {
        if( spaceobject->type != SPACE_PLANET )
          continue;
        if ( !(spaceobject->trainer && (!IS_GOD(ch))) )
          ch_printf( ch, "%s\n\r", spaceobject->name );
        count++;
    }

    if ( !count )
    {
        send_to_char( "There are no spaceobjects currently formed.\n\r", ch );
	return;
    }
}
   
void echo_to_ship( int color , SHIP_DATA *ship , char *argument )
{
     int room;
      
     for ( room = ship->firstroom ; room <= ship->lastroom ;room++ )
     {
         echo_to_room( color , get_room_index(room) , argument );
     }  
     
}

void sound_to_ship( SHIP_DATA *ship , char *argument )
{
     int roomnum;
     ROOM_INDEX_DATA *room;
     CHAR_DATA *vic;
      
     for ( roomnum = ship->firstroom ; roomnum <= ship->lastroom ;roomnum++ )
     {
        room = get_room_index( roomnum );
        if ( room == NULL ) continue;
        
        for ( vic = room->first_person; vic; vic = vic->next_in_room )
        {
	   if ( !IS_NPC(vic) && IS_SET( vic->act, PLR_SOUND ) )
	     send_to_char( argument, vic );
        }
     }  
     
}

void echo_to_docked( int color , SHIP_DATA *ship , char *argument )
{
  SHIP_DATA *dship;
  
  for( dship = first_ship; dship; dship = dship->next )
    if( dship->docked && dship->docked == ship)
      echo_to_cockpit( color, dship, argument );
}

void echo_to_cockpit( int color , SHIP_DATA *ship , char *argument )
{
     int room;
     TURRET_DATA *turret;      
     for ( room = ship->firstroom ; room <= ship->lastroom ;room++ )
     {
         if ( room == ship->cockpit || room == ship->navseat
         || room == ship->pilotseat || room == ship->coseat
         || room == ship->gunseat || room == ship->engineroom )
               echo_to_room( color , get_room_index(room) , argument );
         if( ship->first_turret )
           for( turret = ship->first_turret; turret; turret = turret->next )
             if( turret->roomvnum == room )
               echo_to_room( color , get_room_index(room) , argument );
     }  
}

bool ship_in_range( SHIP_DATA *ship, SHIP_DATA *target )
{
  if (target && ship && target != ship )
    if ( target->spaceobject && ship->spaceobject && 
         target->shipstate != SHIP_LANDED &&
         abs( (int) ( target->vx - ship->vx )) < 100*(ship->mod->sensor+10)*((target->shipclass == SHIP_DEBRIS ? 2 : target->shipclass)+3) &&
         abs( (int) ( target->vy - ship->vy )) < 100*(ship->mod->sensor+10)*((target->shipclass == SHIP_DEBRIS ? 2 : target->shipclass)+3) &&
         abs( (int) ( target->vz - ship->vz )) < 100*(ship->mod->sensor+10)*((target->shipclass == SHIP_DEBRIS ? 2 : target->shipclass)+3) )
      return TRUE;
  return FALSE;
}	

bool missile_in_range( SHIP_DATA *ship, MISSILE_DATA *missile )
{
  if ( missile && ship )
    if ( ship->spaceobject && 
         abs( (int) ( missile->mx - ship->vx )) < 5000 &&
         abs( (int) ( missile->my - ship->vy )) < 5000 &&
         abs( (int) ( missile->mz - ship->vz )) < 5000 )
      return TRUE;
  return FALSE;
}	

bool space_in_range( SHIP_DATA *ship, SPACE_DATA *object )
{
  if (object && ship )
    if ( ship->spaceobject && 
         abs( (int) ( object->xpos - ship->vx )) < 100000 &&
         abs( (int) ( object->ypos - ship->vy )) < 100000 &&
         abs( (int) ( object->zpos - ship->vz )) < 100000 )
      return TRUE;
  return FALSE;
}	
bool space_in_range_c( SHIP_DATA *ship, SPACE_DATA *object )
{
  if (object && ship )
    if ( abs( (int) ( object->xpos - ship->vx )) < 10000 &&
         abs( (int) ( object->ypos - ship->vy )) < 10000 &&
         abs( (int) ( object->zpos - ship->vz )) < 10000 )
      return TRUE;
  return FALSE;
}	
bool space_in_range_h( SHIP_DATA *ship, SPACE_DATA *object )
{
  if (object && ship )
    if ( abs( (int) ( object->xpos - ship->cx )) < (object->gravity*5) &&
         abs( (int) ( object->ypos - ship->cy )) < (object->gravity*5) &&
         abs( (int) ( object->zpos - ship->cz )) < (object->gravity*5) )
      return TRUE;
  return FALSE;
}	

char * print_distance_ship( SHIP_DATA *ship, SHIP_DATA *target )
{
	char *buf;
	float hx, hy, hz;
	hx = target->vx - ship->vx;
	hy = target->vy - ship->vy;
	hz = target->vz - ship->vz;
	
	
	sprintf( buf, "%.0f %.0f %.0f", hx, hy, hz);
	
	return buf;
}
void echo_to_system( int color , SHIP_DATA *ship , char *argument , SHIP_DATA *ignore )
{
     SHIP_DATA *target;

     if (!ship->spaceobject)
        return;

     for ( target = first_ship; target; target = target->next )
     {
       if( !ship_in_range( ship, target ) )
         continue;
       if (target != ship && target != ignore )
         if ( abs( (int) ( target->vx - ship->vx )) < 100*(target->mod->sensor+10)*((ship->shipclass == SHIP_DEBRIS ? 2 : ship->shipclass)+1) &&
         abs( (int) ( target->vy - ship->vy )) < 100*(target->mod->sensor+10)*((ship->shipclass == SHIP_DEBRIS ? 2 : ship->shipclass)+1) &&
         abs( (int) ( target->vz - ship->vz )) < 100*(target->mod->sensor+10)*((ship->shipclass == SHIP_DEBRIS ? 2 : ship->shipclass)+1) )


           echo_to_cockpit( color , target , argument );
     }

}

bool is_facing( SHIP_DATA *ship , SHIP_DATA *target )
{
    	float dy, dx, dz, hx, hy, hz;
        float cosofa;
     	     	
     	hx = ship->hx;
     	hy = ship->hy;
     	hz = ship->hz;
     	     				
	dx = target->vx - ship->vx;
	dy = target->vy - ship->vy;
	dz = target->vz - ship->vz;
     	     							
	cosofa = ( hx*dx + hy*dy + hz*dz ) 
		/ ( sqrt(hx*hx+hy*hy+hz*hz) + sqrt(dx*dx+dy*dy+dz*dz) );
     	     								               
	if ( cosofa > 0.75 )
		return TRUE;
     	     								               	             
	return FALSE;
}
     	     								               	             	

long int get_ship_value( SHIP_DATA *ship )
{
     long int price;
     TURRET_DATA *turret;
          
     if (ship->shipclass == FIGHTER_SHIP)
        price = 5000;
     else if (ship->shipclass == MIDSIZE_SHIP)
     	price = 50000; 
     else if (ship->shipclass == CAPITAL_SHIP) 
        price = 500000;
     else 
        price = 2000;
        
     if ( ship->shipclass <= CAPITAL_SHIP ) 
       price += ( ship->mod->manuever*100*(1+ship->shipclass) );
     
     price += ( ship->mod->tractorbeam * 100 );
     price += ( ship->mod->realspeed * 10 );
     price += ( ship->mod->astro_array *5 );
     price += ( 5 * ship->mod->maxhull );
     price += ( 2 * ship->mod->maxenergy );

     if (ship->mod->maxenergy > 5000 )
          price += ( (ship->mod->maxenergy-5000)*20 ) ;
     
     if (ship->mod->maxenergy > 10000 )
          price += ( (ship->mod->maxenergy-10000)*50 );
     
     if (ship->mod->maxhull > 1000)
        price += ( (ship->mod->maxhull-1000)*10 );
     
     if (ship->mod->maxhull > 10000)
        price += ( (ship->mod->maxhull-10000)*20 );
        
     if (ship->mod->maxshield > 200)
          price += ( (ship->mod->maxshield-200)*50 );
     
     if (ship->mod->maxshield > 1000)
          price += ( (ship->mod->maxshield-1000)*100 );

     if (ship->mod->realspeed > 100 )
        price += ( (ship->mod->realspeed-100)*500 ) ;
        
     if (ship->mod->lasers > 5 )
        price += ( (ship->mod->lasers-5)*500 );
      
     if (ship->mod->maxshield)
     	price += ( 1000 + 10 * ship->mod->maxshield);
     
     if (ship->mod->lasers)
     	price += ( 500 + 500 * ship->mod->lasers );
    
     if (ship->mod->launchers )
     	price += ( 1000 + 100 * ship->mod->launchers);
     
     if (ship->missiles )
     	price += ( 250 * ship->missiles );
     else if (ship->torpedos )
     	price += ( 500 * ship->torpedos );
     else if (ship->rockets )
        price += ( 1000 * ship->rockets );
         
     if( ship->first_turret )
       for( turret = ship->first_turret; turret; turret = turret->next )
         price += 5000;
     
     if (ship->mod->hyperspeed)
        price += ( 1000 + ship->mod->hyperspeed * 10 );
     
     if (ship->hanger)
        price += ( ship->shipclass == MIDSIZE_SHIP ? 50000 : 100000 );
 
     price = (int) ( price*1.5 );
     
     return price;
     
}

void write_ship_list( )
{
    SHIP_DATA *tship;
    FILE *fpout;
    char filename[256];
    
    sprintf( filename, "%s%s", SHIP_DIR, SHIP_LIST );
    fpout = fopen( filename, "w" );
    if ( !fpout )
    {
         bug( "FATAL: cannot open ship.lst for writing!\n\r", 0 );
         return;
    }
    for ( tship = first_ship; tship; tship = tship->next )
    {
      if( tship->shipclass != SHIP_DEBRIS )
        fprintf( fpout, "%s\n", tship->filename );
    }
    fprintf( fpout, "$\n" );
    fclose( fpout );
}
                                                                    
SHIP_DATA * ship_in_room( ROOM_INDEX_DATA *room, char *name )
{
    SHIP_DATA *ship;

    if ( !room )
     return NULL;
     
    for ( ship = first_ship ; ship ; ship = ship->next )
    {
      if( ship->location == room->vnum )
      {
        if( ship->personalname )
          if ( !str_cmp( name, ship->personalname ) )
            return ship;
        if ( !str_cmp( name, ship->name ) )
          return ship;
      }
    }
    for ( ship = first_ship ; ship ; ship = ship->next )
    {
      if( ship->location == room->vnum )
      {
        if( ship->personalname )
          if ( ship->personalname && (ship->personalname[0] != '\0') && nifty_is_name_prefix( name, ship->personalname ) )
            return ship;
        if ( ship->name && (ship->name[0] != '\0') && nifty_is_name_prefix( name, ship->name ) )
          return ship;
      }
    }
    return NULL;    
}

/*
 * Get pointer to ship structure from ship name.
 */
SHIP_DATA *get_ship( char *name )
{
    SHIP_DATA *ship;
    

    for ( ship = first_ship; ship; ship = ship->next )
    {
    if( ship->personalname )
     if ( !str_cmp( name, ship->personalname ) )
         return ship;
     if ( !str_cmp( name, ship->name ) )
         return ship;
    if( ship->personalname )
     if ( ship->personalname && (ship->personalname[0] != '\0') && nifty_is_name_prefix( name, ship->personalname ) )
         return ship;
     if ( ship->name && (ship->name[0] != '\0') && nifty_is_name_prefix( name, ship->name ) )
         return ship;
    }
/*  for ( ship = first_ship; ship; ship = ship->next )
    {
    }
*/
    return NULL;
}

/*
 * Checks if ships in a spaceobject and returns poiner if it is.
 */
SHIP_DATA *get_ship_here( char *name , SHIP_DATA *eShip)
{
    SHIP_DATA *ship;
    int number, count = 0;
    char arg[MAX_INPUT_LENGTH];

    if ( eShip == NULL )
         return NULL;

    number = number_argument( name, arg );
    
    for ( ship = first_ship ; ship; ship = ship->next )
    {
     if( !ship_in_range( eShip, ship ) )
       continue;
     if( !ship->spaceobject )
       continue;
     if( ship->personalname )
      if ( !str_cmp( arg, ship->personalname ) )
      {
       count++;
       if( !number || count == number )
         return ship;
     }
     if ( !str_cmp( arg, ship->name ) )
     {
       count++;
       if( !number ||  count == number )
         return ship;
     }
    }
    count = 0;
    for ( ship = first_ship; ship; ship = ship->next )
    {
     if( !ship_in_range( eShip, ship ) )
       continue;
     if( ship->personalname )
      if ( ship->personalname && (ship->personalname[0] != '\0') && nifty_is_name_prefix( arg, ship->personalname ) )
      {
       count++;
       if(  !number || count == number )
         return ship;
     }
     if ( ship->name && (ship->name[0] != '\0') && nifty_is_name_prefix( arg, ship->name ) )
     {
       count++;
       if(  !number || count == number )
         return ship;
     }
    }
    return NULL;
}

/*
 * Get pointer to ship structure from ship filename.
 */
SHIP_DATA *get_ship_from_filename( char *name )
{
    SHIP_DATA *ship;
    

    for ( ship = first_ship; ship; ship = ship->next )
    {
     if ( !str_cmp( name, ship->filename ) )
         return ship;
     if ( ship->filename && (ship->filename[0] != '\0') && nifty_is_name_prefix( name, ship->filename) )
         return ship;
    }
/*  for ( ship = first_ship; ship; ship = ship->next )
    {
    }
*/
    return NULL;
}

/*
 * Get pointer to ship structure from pilot name.
 */
SHIP_DATA *ship_from_pilot( char *name )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( !str_cmp( name, ship->pilot ) )
         return ship;
       if ( !str_cmp( name, ship->copilot ) )
         return ship;
       if ( !str_cmp( name, ship->owner ) )
         return ship;  
    return NULL;
}

/*
 * Get pointer to # ship structure from pilot name.
 */
SHIP_DATA *ship_from_pilot_num( char *name, int num )
{
    SHIP_DATA *ship;
    int count = 1;
    
    if ( !num )
      num = 1;
    for ( ship = first_ship; ship; ship = ship->next )
    {
       if ( !str_cmp( name, ship->pilot ) && count == num )
         return ship;
       if ( !str_cmp( name, ship->copilot ) && count == num )
         return ship;
       if ( !str_cmp( name, ship->owner ) && count == num )
         return ship;  
       count++;
    }
    return NULL;
}

/*
 * Get pointer to ship structure from cockpit, turret, or entrance ramp vnum.
 */
 
SHIP_DATA *ship_from_cockpit( int vnum )
{
    SHIP_DATA *ship;
    TURRET_DATA *turret;
    
    for ( ship = first_ship; ship; ship = ship->next )
    {
       if ( vnum == ship->cockpit || vnum == ship->hanger
          || vnum == ship->pilotseat || vnum == ship->coseat || vnum == ship->navseat
          || vnum == ship->gunseat  || vnum == ship->engineroom )
         return ship;
      if( ship->first_turret )
        for( turret = ship->first_turret; turret; turret = turret->next )
          if( turret->roomvnum == vnum )
            return ship;
      if( ship->cockpitroom && ship->cockpitroom->vnum == vnum )
        return ship;
    }
    return NULL;
}

SHIP_DATA *ship_from_pilotseat( int vnum )
{
    SHIP_DATA *ship;

    for ( ship = first_ship; ship; ship = ship->next )
    {
       if ( vnum == ship->pilotseat )
         return ship;
      if( ship->cockpitroom && ship->cockpitroom->vnum == vnum )
        return ship;
    }

    return NULL;
}

SHIP_DATA *ship_from_coseat( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
    {
      if ( vnum == ship->coseat )
         return ship;
      if( ship->cockpitroom && ship->cockpitroom->vnum == vnum )
        return ship;
    }

    return NULL;
}

SHIP_DATA *ship_from_navseat( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
    {
      if ( vnum == ship->navseat )
         return ship;
      if( ship->cockpitroom && ship->cockpitroom->vnum == vnum )
        return ship;
    }
    
    return NULL;
}

SHIP_DATA *ship_from_gunseat( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
    {
      if ( vnum == ship->gunseat )
        return ship;
      if( ship->cockpitroom && ship->cockpitroom->vnum == vnum )
        return ship;
    }
    return NULL;
}

SHIP_DATA *ship_from_engine( int vnum )
{
    SHIP_DATA *ship;
    TURRET_DATA *turret;
    
    for ( ship = first_ship; ship; ship = ship->next )
    {   
      if (ship->engineroom)   
      {  
        if ( vnum == ship->engineroom )
          return ship;
      }
      else
      { 
        if ( vnum == ship->cockpit )
          return ship;
      }
      if ( ship->first_turret )
        for ( turret = ship->first_turret; turret; turret = turret->next )
          if ( vnum == turret->roomvnum )
            return ship;
      if( ship->cockpitroom && ship->cockpitroom->vnum == vnum )
        return ship;
    }

    return NULL;
}



SHIP_DATA *ship_from_turret( int vnum )
{
    SHIP_DATA *ship;
    TURRET_DATA *turret;
    
    for ( ship = first_ship; ship; ship = ship->next )
    {
       if ( vnum == ship->gunseat )
         return ship;
      if( ship->first_turret )
        for( turret = ship->first_turret; turret; turret = turret->next )
          if( turret->roomvnum == vnum )
            return ship;
      if( ship->cockpitroom && ship->cockpitroom->vnum == vnum )
        return ship;
    }

    return NULL;
}

SHIP_DATA *ship_from_entrance( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
    {
       if ( vnum == ship->entrance )
         return ship;
      if( ship->cockpitroom && ship->cockpitroom->vnum == vnum )
        return ship;
    }
    return NULL;
}

SHIP_DATA *ship_from_hanger( int vnum )
{
    SHIP_DATA *ship;
    
    if ( vnum )
     for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum == ship->hanger )
         return ship;
    return NULL;
}

SHIP_DATA *ship_from_room( int vnum )
{
    SHIP_DATA *ship;
    
    for ( ship = first_ship; ship; ship = ship->next )
       if ( vnum >= ship->firstroom && vnum <= ship->lastroom )
         return ship;
    return NULL;
}

void save_ship( SHIP_DATA *ship )
{
    FILE *fp;
    char filename[256];
    char buf[MAX_STRING_LENGTH];
    MODULE_DATA *module;
    TURRET_DATA *turret;

    if ( !ship )
    {
	bug( "save_ship: null ship pointer!", 0 );
	return;
    }
    
    update_ship_modules( ship );
    
    if ( ship->shipclass == SHIP_DEBRIS || ship->cockpitroom )
      return;
        
    if ( !ship->filename || ship->filename[0] == '\0' )
    {
	sprintf( buf, "save_ship: %s has no filename", ship->name );
	bug( buf, 0 );
	return;
    }

    sprintf( filename, "%s%s", SHIP_DIR, ship->filename );
    
    fclose( fpReserve );
    if ( ( fp = fopen( filename, "w" ) ) == NULL )
    {
    	bug( "save_ship: fopen", 0 );
    	perror( filename );
    }
    else
    {
	fprintf( fp, "#SHIP\n" );
	fprintf( fp, "Name         %s~\n",	ship->name		);
	fprintf( fp, "PersonalName         %s~\n",	ship->personalname		);
	fprintf( fp, "Filename     %s~\n",	ship->filename		);
	fprintf( fp, "ShipID	   %ld\n",	ship->shipID		);
        fprintf( fp, "Description  %s~\n",	ship->description	);
	fprintf( fp, "Owner        %s~\n",	ship->owner		);
	fprintf( fp, "Pilot        %s~\n",      ship->pilot             );
	fprintf( fp, "Copilot      %s~\n",      ship->copilot           );
	fprintf( fp, "Class        %d\n",	ship->shipclass		);
	fprintf( fp, "Tractorbeam  %d\n",	ship->tractorbeam	);
	fprintf( fp, "Shipyard     %d\n",	ship->shipyard		);
	fprintf( fp, "Hanger       %d\n",	ship->hanger    	);
//	fprintf( fp, "Cargohold       %d\n",	ship->cargohold		);
	fprintf( fp, "Vx           %.0f\n",	ship->vx        	);
	fprintf( fp, "Vy           %.0f\n",	ship->vy        	);
	fprintf( fp, "Vz           %.0f\n",	ship->vz        	);
/*	fprintf( fp, "Turret1      %d\n",	ship->turret1		);
	fprintf( fp, "Turret2      %d\n",	ship->turret2		);
	fprintf( fp, "Turret3      %d\n",	ship->turret3		);
	fprintf( fp, "Turret4      %d\n",	ship->turret4		);
	fprintf( fp, "Turret5      %d\n",	ship->turret5		);
	fprintf( fp, "Turret6      %d\n",	ship->turret6		);
	fprintf( fp, "Turret7      %d\n",	ship->turret7		);
	fprintf( fp, "Turret8      %d\n",	ship->turret8		);
	fprintf( fp, "Turret9      %d\n",	ship->turret9		);
	fprintf( fp, "Turret0      %d\n",	ship->turret0		);
*/
	fprintf( fp, "Statet0      %d\n",	ship->statet0		);
	fprintf( fp, "Statei0      %d\n",	ship->statei0		);
	fprintf( fp, "Lasers       %d\n",	ship->lasers    	);
	fprintf( fp, "Missiles     %d\n",	ship->missiles		);
	fprintf( fp, "Maxmissiles  %d\n",	ship->maxmissiles	);
	fprintf( fp, "Rockets     %d\n",	ship->rockets		);
	fprintf( fp, "Maxrockets  %d\n",	ship->maxrockets	);
	fprintf( fp, "Torpedos     %d\n",	ship->torpedos		);
	fprintf( fp, "Maxtorpedos  %d\n",	ship->maxtorpedos	);
	fprintf( fp, "Lastdoc      %d\n",	ship->lastdoc		);
	fprintf( fp, "Firstroom    %d\n",	ship->firstroom		);
	fprintf( fp, "Lastroom     %d\n",	ship->lastroom		);
	fprintf( fp, "Shield       %d\n",	ship->shield		);
	fprintf( fp, "Maxshield    %d\n",	ship->maxshield		);
	fprintf( fp, "Hull         %d\n",	ship->hull		);
	fprintf( fp, "Maxhull      %d\n",	ship->maxhull		);
	fprintf( fp, "Maxenergy    %d\n",	ship->maxenergy		);
	fprintf( fp, "Hyperspeed   %d\n",	ship->hyperspeed	);
	fprintf( fp, "Comm         %d\n",	ship->comm		);
	fprintf( fp, "Chaff        %d\n",	ship->chaff		);
	fprintf( fp, "Maxchaff     %d\n",	ship->maxchaff		);
	fprintf( fp, "Sensor       %d\n",	ship->sensor		);
	fprintf( fp, "Astro_array  %d\n",	ship->astro_array	);
	fprintf( fp, "Realspeed    %d\n",	ship->realspeed		);
	fprintf( fp, "Type         %d\n",	ship->type		);
	fprintf( fp, "Cockpit      %d\n",	ship->cockpit		);
	fprintf( fp, "Coseat       %d\n",	ship->coseat		);
	fprintf( fp, "Pilotseat    %d\n",	ship->pilotseat		);
	fprintf( fp, "Gunseat      %d\n",	ship->gunseat		);
	fprintf( fp, "Navseat      %d\n",	ship->navseat		);
	fprintf( fp, "Engineroom   %d\n",       ship->engineroom        );
	fprintf( fp, "Entrance     %d\n",       ship->entrance          );
	fprintf( fp, "Shipstate    %d\n",	ship->shipstate		);
	fprintf( fp, "Missilestate %d\n",	ship->missilestate	);
	fprintf( fp, "Energy       %d\n",	ship->energy		);
	fprintf( fp, "Manuever     %d\n",       ship->manuever          );
	fprintf( fp, "Alarm        %d\n",       ship->alarm             );
	fprintf( fp, "UpgradeBlock %d\n",	ship->upgradeblock    	);
   fprintf( fp, "Ions         %d\n",       ship->ions              );
   fprintf( fp, "MaxIntModules   %d\n",		ship->maxintmodules    	);
   fprintf( fp, "MaxExtModules   %d\n",		ship->maxextmodules    	);
   fprintf( fp, "Maxcargo     %d\n",       ship->maxcargo              );
   fprintf( fp, "Weight         %d\n",       ship->weight              );
/* fprintf( fp, "Cargo0         %d\n",       ship->cargo0              );
   fprintf( fp, "Cargo1         %d\n",       ship->cargo1              );
   fprintf( fp, "Cargo2         %d\n",       ship->cargo2              );
   fprintf( fp, "Cargo3         %d\n",       ship->cargo3              );
   fprintf( fp, "Cargo4         %d\n",       ship->cargo4              );
   fprintf( fp, "Cargo5         %d\n",       ship->cargo5              );
   fprintf( fp, "Cargo6         %d\n",       ship->cargo6              );
   fprintf( fp, "Cargo7         %d\n",       ship->cargo7              );
   fprintf( fp, "Cargo8         %d\n",       ship->cargo8              );
   fprintf( fp, "Cargo9         %d\n",       ship->cargo9              );
   fprintf( fp, "CaughtSmug     %d\n",       ship->caughtsmug              );
*/
   fprintf( fp, "Dockingports   %d\n",       ship->dockingports        );
   fprintf( fp, "Guard   %d\n",		     ship->guard        );
   fprintf( fp, "Home         %s~\n",      ship->home              );
   if( !ship->templatestring )
     ship->templatestring = STRALLOC( "" );
   fprintf( fp, "Templatestring %s~\n",	     ship->templatestring );
   fprintf( fp, "End\n"						);
   if ( ship->first_module)
   {
		for( module = ship->first_module; module; module = module->next )
		{
			fprintf( fp, "#MODS\n"					);
			fprintf( fp, "Name %s~\n\rMod %d %d %d %d~\n", 
			   module->name, module->type, module->condition, module->size, module->modification);	
			fprintf( fp, "End\n"					);
		}
   }

   if ( ship->first_turret )
   {
		for( turret = ship->first_turret; turret; turret = turret->next )
		{
			fprintf( fp, "#MODS\n"					);
			fprintf( fp, "Name %s~\n\rMod %d %d %d %d~\n", 
			   "Turret", MOD_TURRET, turret->type, turret->roomvnum, turret->state );	
			fprintf( fp, "End\n"					);
		}
   }


  }
    
  /*
    if ( ship->cargohold )
    {
       	OBJ_DATA *contents;
	ROOM_INDEX_DATA *cargoroom;
  	cargoroom = get_room_index(ship->cargohold);

    	if ( cargoroom != NULL )
    	{
        	contents = cargoroom->last_content;
//		fprintf( fp, "#CARGO\n" );

        	if (contents)
	  	  fwrite_obj(NULL, contents, fp, 0, OS_CARRY );
		fprintf( fp, "END\n" );
		fprintf( fp, "#END\n"						);
    	}
    }
*/    
    
    fclose( fp );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}


/*
 * Read in actual ship data.
 */

#if defined(KEY)
#undef KEY
#endif

#define KEY( literal, field, value )					\
				if ( !str_cmp( word, literal ) )	\
				{					\
				    field  = value;			\
				    fMatch = TRUE;			\
				    break;				\
				}

#if 0
void fread_cargohold( SHIP_DATA *ship, FILE *fp )
{

         ROOM_INDEX_DATA *storeroom = get_room_index(ship->cargohold);
         OBJ_DATA *obj;
         OBJ_DATA *obj_next;
           
	 if( !storeroom )
	   return;
	   
         for ( obj = storeroom->first_content; obj; obj = obj_next )
	 {
	    obj_next = obj->next_content;
	    extract_obj( obj );
	 }

	 if ( fp != NULL )
	 {
	    int iNest;
	    bool found;
	    OBJ_DATA *tobj, *tobj_next;

	    rset_supermob(storeroom);

	    found = TRUE;
	    for ( ; ; )
	    {
		char letter;
		char *word;

		letter = fread_letter( fp );
		if ( letter == '*' )
		{
		    fread_to_eol( fp );
		    continue;
		}

		if ( letter != '#' )
		{
		    bug( "fread_cargohold: # not found.", 0 );
		    bug( &letter, 0 );
		    bug( ship->name, 0 );
		    break;
		}

		word = fread_word( fp );
		if ( !str_cmp( word, "OBJECT" ) )	/* Objects	*/
		{
 		  bug( "Read an object!.", 0 );
		  fread_obj  ( supermob, fp, OS_CARRY );
		}
		else
		if ( !str_cmp( word, "END"    ) )	/* Done		*/
		  break;
		else
		{
		    bug( "fread_cargohold: bad section.", 0 );
		    bug( ship->name, 0 );
		    break;
		}
	    }

	    fclose( fp );

	    for ( tobj = supermob->first_carrying; tobj; tobj = tobj_next )
	    {
		tobj_next = tobj->next_content;
		obj_from_char( tobj );
                if( tobj->item_type != ITEM_MONEY )
		  obj_to_room( tobj, storeroom );
	    }
	    
	    release_supermob();

         }
}
#endif

void fread_ship( SHIP_DATA *ship, FILE *fp )
{
    char buf[MAX_STRING_LENGTH];
    const char *word;
    bool fMatch;
    int dummy_number, turretvnum;
    SHIP_MOD_DATA *ship_mod;
    TURRET_DATA *turret;

    for ( ; ; )
    {
	word   = feof( fp ) ? "End" : fread_word( fp );
	fMatch = FALSE;
        
	switch ( UPPER(word[0]) )
	{
	case '*':
	    fMatch = TRUE;
	    fread_to_eol( fp );
	    break;
        
        case 'A':
             KEY( "Astro_array",      ship->astro_array,       fread_number( fp ) );
             KEY( "Alarm",            ship->alarm,             fread_number( fp ) );
             break;
        
        case 'C':
/*           KEY( "Cargo0",  ship->cargo0, fread_number( fp ) );
             KEY( "Cargo1",  ship->cargo1, fread_number( fp ) );
             KEY( "Cargo2",  ship->cargo2, fread_number( fp ) );
             KEY( "Cargo3",  ship->cargo3, fread_number( fp ) );
             KEY( "Cargo4",  ship->cargo4, fread_number( fp ) );
             KEY( "Cargo5",  ship->cargo5, fread_number( fp ) );
             KEY( "Cargo6",  ship->cargo6, fread_number( fp ) );
             KEY( "Cargo7",  ship->cargo7, fread_number( fp ) );
             KEY( "Cargo8",  ship->cargo8, fread_number( fp ) );
             KEY( "Cargo9",  ship->cargo9, fread_number( fp ) );
             KEY( "Cargohold",  ship->cargohold, fread_number( fp ) );
             KEY( "CaughtSmug",  ship->caughtsmug, fread_number( fp ) );
*/
             KEY( "Cockpit",     ship->cockpit,          fread_number( fp ) );
             KEY( "Coseat",     ship->coseat,          fread_number( fp ) );
             KEY( "Class",       ship->shipclass,            fread_number( fp ) );
             KEY( "Copilot",     ship->copilot,          fread_string( fp ) );
             KEY( "Comm",        ship->comm,      fread_number( fp ) );
             KEY( "Chaff",       ship->chaff,      fread_number( fp ) );
             break;
                                

	case 'D':
	    KEY( "Description",	ship->description,	fread_string( fp ) );
	    KEY( "DockingPorts",    ship->dockingports,      fread_number( fp ) );
	    break;

	case 'E':
	    KEY( "Engineroom",    ship->engineroom,      fread_number( fp ) );
	    KEY( "Entrance",	ship->entrance,	        fread_number( fp ) );
	    KEY( "Energy",      ship->energy,        fread_number( fp ) );
	    if ( !str_cmp( word, "End" ) )
	    {
		if (!ship->home)
		  ship->home		= STRALLOC( "" );
		if (!ship->name)
		  ship->name		= STRALLOC( "" );
		if (!ship->owner)
		  ship->owner		= STRALLOC( "" );
		if (!ship->description)
		  ship->description 	= STRALLOC( "" );
		if (!ship->copilot)
		  ship->copilot 	= STRALLOC( "" );
		if (!ship->pilot)
		  ship->pilot   	= STRALLOC( "" );
		if (ship->shipstate != SHIP_DISABLED)
		  ship->shipstate = SHIP_LANDED;
		if (ship->statet0 != LASER_DAMAGED)
		  ship->statet0 = LASER_READY;
		if (ship->statei0 != LASER_DAMAGED)
		  ship->statei0 = LASER_READY;
		if (ship->missilestate != MISSILE_DAMAGED)
		  ship->missilestate = MISSILE_READY;
		if (ship->shipyard <= 0)
		  ship->shipyard = ROOM_LIMBO_SHIPYARD;
		if (ship->lastdoc <= 0) 
		  ship->lastdoc = ship->shipyard;
		ship->bayopen     = FALSE;
		ship->autopilot   = FALSE;
		ship->hatchopen = FALSE;
		ship->tractored    = NULL;
		ship->tractoredby = NULL;
		if (ship->navseat <= 0)
		  ship->navseat = ship->cockpit;
		if (ship->gunseat <= 0)
		  ship->gunseat = ship->cockpit;
		if (ship->coseat <= 0) 
		  ship->coseat = ship->cockpit;
		if (ship->pilotseat <= 0)
		  ship->pilotseat = ship->cockpit;
		if (ship->missiletype == 1)
		{
		  ship->torpedos = ship->missiles;    /* for back compatability */
		  ship->missiles = 0;
		}
		ship->spaceobject = NULL;
		ship->in_room=NULL;
                ship->next_in_room=NULL;
                ship->prev_in_room=NULL;
		ship->tcount = 0;
		ship->modules=0;
		if (ship->maxextmodules == 0)
			ship->maxextmodules = 10;
		if (ship->maxintmodules == 0)
			ship->maxintmodules = 3;
		
		if( ship->mod == NULL )
		{
 		  CREATE( ship_mod, SHIP_MOD_DATA, 1 );
		  ship->mod = ship_mod;
		  update_ship_modules(ship);
		}
			
	        if( ship->shipclass < 3 )
                  ship->bayopen = FALSE;
                  
                if ( !ship->templatestring )
                  ship->templatestring = STRALLOC( "" );
                  
                if( ship->templatestring[0] != '\0' )
                    if ( parse_ship_template(ship->templatestring, ship) )
		      bug( "fread_ship: parse_ship_template failed.\n\r", 0 );

		return;
	    }
	    break;

	case 'F':
	    KEY( "Filename",	ship->filename,		fread_string_nohash( fp ) );
            KEY( "Firstroom",   ship->firstroom,        fread_number( fp ) );
            break;
        
        case 'G':    
            KEY( "Guard",     ship->guard,          fread_number( fp ) );
            KEY( "Gunseat",     ship->gunseat,          fread_number( fp ) );
            break;
        
        case 'H':
            KEY( "Home" , ship->home, fread_string( fp ) );
            KEY( "Hyperspeed",   ship->hyperspeed,      fread_number( fp ) );
            KEY( "Hull",      ship->hull,        fread_number( fp ) );
            KEY( "Hanger",  ship->hanger,      fread_number( fp ) );
            break;

        case 'I':
            KEY( "Ions" , ship->ions, fread_number( fp ) );
            break;

        case 'L':
            KEY( "Laserstr",   ship->lasers,   (sh_int)  ( fread_number( fp )/10 ) );
            KEY( "Lasers",   ship->lasers,      fread_number( fp ) );
            KEY( "Lastdoc",    ship->lastdoc,       fread_number( fp ) );
            KEY( "Lastroom",   ship->lastroom,        fread_number( fp ) );
            break;

        case 'M':
            KEY( "Manuever",   ship->manuever,      fread_number( fp ) );
            KEY( "Maxcargo",   ship->maxcargo,      fread_number( fp ) );
	    KEY( "MaxIntModules",		ship->maxintmodules,	fread_number( fp ) );
	    KEY( "MaxExtModules",		ship->maxextmodules,	fread_number( fp ) );
            KEY( "Maxmissiles",   ship->maxmissiles,      fread_number( fp ) );
            KEY( "Maxtorpedos",   ship->maxtorpedos,      fread_number( fp ) );
            KEY( "Maxrockets",   ship->maxrockets,      fread_number( fp ) );
            KEY( "Missiles",   ship->missiles,      fread_number( fp ) );
            KEY( "Missiletype",   ship->missiletype,      fread_number( fp ) );
            KEY( "Maxshield",      ship->maxshield,        fread_number( fp ) );
            KEY( "Maxenergy",      ship->maxenergy,        fread_number( fp ) );
            KEY( "Missilestate",   ship->missilestate,        fread_number( fp ) );
            KEY( "Maxhull",      ship->maxhull,        fread_number( fp ) );
            KEY( "Maxchaff",       ship->maxchaff,      fread_number( fp ) );
             break;

	case 'N':
	    KEY( "Name",	ship->name,		fread_string( fp ) );
	    KEY( "Navseat",     ship->navseat,          fread_number( fp ) );
            break;
  
        case 'O':
            KEY( "Owner",            ship->owner,            fread_string( fp ) );
            KEY( "Objectnum",        dummy_number,        fread_number( fp ) );
            break;

        case 'P':
	    KEY( "PersonalName",	ship->personalname,		fread_string( fp ) );
            KEY( "Pilot",            ship->pilot,            fread_string( fp ) );
            KEY( "Pilotseat",     ship->pilotseat,          fread_number( fp ) );
            break;
        
        case 'R':
            KEY( "Realspeed",   ship->realspeed,       fread_number( fp ) );
            KEY( "Rockets",     ship->rockets,         fread_number( fp ) );
            break;
       
        case 'S':
            KEY( "Shipyard",    ship->shipyard,      fread_number( fp ) );
            KEY( "Sensor",      ship->sensor,       fread_number( fp ) );
            KEY( "Shield",      ship->shield,        fread_number( fp ) );
            KEY( "ShipID",      ship->shipID,        fread_number( fp ) );
            KEY( "Shipstate",   ship->shipstate,        fread_number( fp ) );
            KEY( "Statei0",   ship->statet0,        fread_number( fp ) );
            KEY( "Statet0",   ship->statet0,        fread_number( fp ) );
            break;

	case 'T':
	    KEY( "Templatestring", ship->templatestring, fread_string( fp ) );
	    KEY( "Torpedos",	ship->torpedos,	fread_number( fp ) );
	    KEY( "Type",	ship->type,	fread_number( fp ) );
	    KEY( "Tractorbeam", ship->tractorbeam,      fread_number( fp ) );
	    if ( !str_cmp( word, "Turret1" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}
	    if ( !str_cmp( word, "Turret2" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}
	    if ( !str_cmp( word, "Turret3" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}
	    if ( !str_cmp( word, "Turret4" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}
	    if ( !str_cmp( word, "Turret5" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}
	    if ( !str_cmp( word, "Turret6" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}
	    if ( !str_cmp( word, "Turret7" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}
	    if ( !str_cmp( word, "Turret8" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}
	    if ( !str_cmp( word, "Turret9" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}
	    if ( !str_cmp( word, "Turret0" ) )
		{
		  turretvnum = fread_number( fp );
		  if( turretvnum == 0 ) break;
		  CREATE( turret, TURRET_DATA, 1 );
		  turret->type	 	= 0;
		  turret->roomvnum	= turretvnum;
		  turret->state		= LASER_READY;

		  LINK( turret, ship->first_turret, ship->last_turret, next, prev );
		}

/*	    KEY( "Turret1",	ship->turret1,	fread_number( fp ) );
	    KEY( "Turret2",	ship->turret2,	fread_number( fp ) );
       KEY( "Turret3",  ship->turret3, fread_number( fp ) );
       KEY( "Turret4",  ship->turret4, fread_number( fp ) );
       KEY( "Turret5",  ship->turret5, fread_number( fp ) );
       KEY( "Turret6",  ship->turret6, fread_number( fp ) );
       KEY( "Turret7",  ship->turret7, fread_number( fp ) );
       KEY( "Turret8",  ship->turret8, fread_number( fp ) );
       KEY( "Turret9",  ship->turret9, fread_number( fp ) );
       KEY( "Turret0",  ship->turret0, fread_number( fp ) );
*/
	    break;
	
	case 'U':
	    KEY( "UpgradeBlock",          ship->upgradeblock,     fread_number( fp ) );
	    break;
	
	case 'V':
	    KEY( "Vx",          ship->vx,     fread_number( fp ) );
	    KEY( "Vy",          ship->vy,     fread_number( fp ) );
	    KEY( "Vz",          ship->vz,     fread_number( fp ) );
	    break;

	case 'W':
	    KEY( "Weight",          ship->weight,     fread_number( fp ) );
	    break;
	}
	
	if ( !fMatch )
	{
	    sprintf( buf, "Fread_ship: no match: %s", word );
	    bug( buf, 0 );
	}
    }
}

/*
 * Load a ship file
 */

bool load_ship_file( char *shipfile )
{
    char filename[256];
    SHIP_DATA *ship;
    FILE *fp;
    bool found, isbus = FALSE;
    int bus;
    ROOM_INDEX_DATA *pRoomIndex;
    CLAN_DATA *clan;

    CREATE( ship, SHIP_DATA, 1 );

    found = FALSE;
    sprintf( filename, "%s%s", SHIP_DIR, shipfile );

    if ( ( fp = fopen( filename, "r" ) ) != NULL )
    {

	found = TRUE;
	for ( ; ; )
	{
	    char letter;
	    char *word;

	    letter = fread_letter( fp );
	    if ( letter == '*' )
	    {
		fread_to_eol( fp );
		continue;
	    }

	    if ( letter != '#' )
	    {
		bug( "Load_ship_file: # not found.", 0 );
		break;
	    }

	    word = fread_word( fp );
	    if ( !str_cmp( word, "SHIP"	) )
	    {
	    	fread_ship( ship, fp );
//		fread_cargohold( ship, fp );
	    }
	    else if ( !str_cmp( word, "MODS" ) )	
		{		
//			log_string( "Read Modules" );
			fread_modules( ship, fp );
			update_ship_modules( ship );
		}
	    else
	    if ( !str_cmp( word, "END"	) )
	        break;
	    else
	    {
		char buf[MAX_STRING_LENGTH];

		sprintf( buf, "Load_ship_file: bad section: %s.", word );
		bug( buf, 0 );
		break;
	    }
	}
	fclose( fp );
    }
    if ( !(found) )
      DISPOSE( ship );
    else
    {
       LINK( ship, first_ship, last_ship, next, prev );
       
       update_ship_modules( ship );
       
       ship->docking = SHIP_READY;
       if( !(ship->dockingports) )
         ship->dockingports = 0;

       for( bus = 0; bus < MAX_BUS; bus++ )
         if( ship->cockpit == serin[bus].cockpitvnum )
           isbus = TRUE;

       if ( ( !str_cmp("Trainer", ship->owner) || !str_cmp("Public",ship->owner) || ship->type == MOB_SHIP ) && !isbus )
       {

         if ( ship->shipclass != SHIP_PLATFORM && ship->type != MOB_SHIP && ship->shipclass != CAPITAL_SHIP )
         {
           extract_ship( ship );
           ship_to_room( ship , ship->shipyard );

           ship->location = ship->shipyard;
           ship->lastdoc = ship->shipyard;
           ship->shipstate = SHIP_LANDED;
           ship->docking = SHIP_READY;
           }

     if( ship->personalname == NULL )
       ship->personalname = STRALLOC(ship->name);

     ship->currspeed=0;
     ship->energy=ship->mod->maxenergy;
     ship->hull=ship->mod->maxhull;
     ship->shield=0;

     ship->statet0 = LASER_READY;
     ship->missilestate = LASER_READY;
     ship->statettractor = SHIP_READY;
     ship->statetdocking = SHIP_READY;
     ship->docking = SHIP_READY;

     ship->currjump=NULL;
     ship->target0=NULL;

     ship->hatchopen = FALSE;
     ship->bayopen = FALSE;

     ship->autorecharge = FALSE;
     ship->autotrack = FALSE;
     ship->autospeed = FALSE;


       }

       else if ( ship->cockpit == ROOM_SENATE_SHUTTLE ||
                 ship->cockpit == ROOM_CORUSCANT_TURBOCAR ||
                 ship->cockpit == ROOM_CORUSCANT_SHUTTLE ||
                 isbus  )
       {}
       else if ( ( pRoomIndex = get_room_index( ship->lastdoc ) ) != NULL
            && ship->shipclass != CAPITAL_SHIP && ship->shipclass != SHIP_PLATFORM && ship->type != MOB_SHIP )
       {
              LINK( ship, pRoomIndex->first_ship, pRoomIndex->last_ship, next_in_room, prev_in_room );
              ship->in_room = pRoomIndex;
              ship->location = ship->lastdoc;
       }


       if ( ship->shipclass == SHIP_PLATFORM || ship->type == MOB_SHIP || ship->shipclass == CAPITAL_SHIP )
       {
 
          ship_to_spaceobject(ship, spaceobject_from_name(ship->home) );
/*  Do not need now that coordinates are saved in ship files.
          ship->vx = number_range( -5000 , 5000 );
          ship->vy = number_range( -5000 , 5000 );
          ship->vz = number_range( -5000 , 5000 );
 */ 
          ship->hx = 1;
          ship->hy = 1;
          ship->hz = 1;

    if( ship->vx == 0 && ship->vy == 0 && ship->vz == 0 )
     if ( ( ship->shipclass == SHIP_PLATFORM || ship->type == MOB_SHIP || ship->shipclass == CAPITAL_SHIP )
          && ship->home )
     {
          ship_to_spaceobject(ship, spaceobject_from_name(ship->home) );
          ship->vx = number_range( -5000 , 5000 );
          ship->vy = number_range( -5000 , 5000 );
          ship->vz = number_range( -5000 , 5000 );
	  if( ship->spaceobject )
	  {
	    ship->vx += ship->spaceobject->xpos;
	    ship->vy += ship->spaceobject->ypos;
	    ship->vz += ship->spaceobject->zpos;
	  }
          ship->shipstate = SHIP_READY;
          ship->autopilot = TRUE;
          ship->autorecharge = TRUE;
          ship->shield = ship->maxshield;
     }
          
          ship->shipstate = SHIP_READY;
          ship->docking = SHIP_READY;
          ship->autopilot = TRUE;
          ship->autorecharge = TRUE;
          ship->shield = ship->maxshield;
       }

     if ( ship->type == MOB_SHIP )
     {
       ship->location = 0;
       ship->lastdoc = 0;
     }
       

         if ( ship->type != MOB_SHIP && (clan = get_clan( ship->owner )) != NULL )
         {
          if ( ship->shipclass <= SHIP_PLATFORM )
             clan->spacecraft++;
          else
             clan->vehicles++;
         }
       ship->docking = SHIP_READY;

    }

    return found;
}

/*
 * Load in all the ship files.
 */
void load_ships( )
{
    FILE *fpList;
    const char *filename;
    char shiplist[256];
    char buf[MAX_STRING_LENGTH];
    SHIP_DATA *ship;
    ROOM_INDEX_DATA *pRoomIndex;


    first_ship	= NULL;
    last_ship	= NULL;
    first_missile = NULL;
    last_missile = NULL;

    log_string( "Loading ships..." );

    sprintf( shiplist, "%s%s", SHIP_DIR, SHIP_LIST );
    fclose( fpReserve );
    if ( ( fpList = fopen( shiplist, "r" ) ) == NULL )
    {
	perror( shiplist );
	exit( 1 );
    }

    for ( ; ; )
    {

	filename = feof( fpList ) ? "$" : fread_word( fpList );

	if ( filename[0] == '$' )
	  break;

	if ( !load_ship_file( (char*) filename ) )
	{
	  sprintf( buf, "Cannot load ship file: %s", filename );
	  bug( buf, 0 );
	}

    }
    fclose( fpList );

    for( ship = first_ship; ship; ship = ship->next )
      if( !ship->location )
        if ( ( pRoomIndex = get_room_index( ship->lastdoc ) ) != NULL
            && ship->shipclass != CAPITAL_SHIP && ship->shipclass != SHIP_PLATFORM )
        {

              LINK( ship, pRoomIndex->first_ship, pRoomIndex->last_ship, next_in_room, prev_in_room );
              ship->in_room = pRoomIndex;
              ship->location = ship->lastdoc;
        }


    log_string(" Done ships " );
    fpReserve = fopen( NULL_FILE, "r" );
    return;
}

void resetship( SHIP_DATA *ship )
{
     ship->shipstate = SHIP_READY;
     ship->docking = SHIP_READY;
     ship->docked = NULL;
     if ( (ship->shipclass != SHIP_PLATFORM && ship->shipclass != CAPITAL_SHIP) && ship->type != MOB_SHIP )
     {
           extract_ship( ship );
           ship_to_room( ship , ship->shipyard ); 
     
           ship->location = ship->shipyard;
           ship->lastdoc = ship->shipyard; 
           ship->shipstate = SHIP_LANDED;
     }
     
     if (ship->spaceobject)
        ship_from_spaceobject( ship, ship->spaceobject );

     ship->currspeed=0;
     ship->energy=ship->mod->maxenergy;
     ship->hull=ship->mod->maxhull;
     ship->shield=0;

     ship->statet0 = LASER_READY;
     ship->missilestate = LASER_READY;

     ship->currjump=NULL;
     ship->target0=NULL;

     ship->hatchopen = FALSE;
     ship->bayopen = FALSE;

     ship->autorecharge = FALSE;
     ship->autotrack = FALSE;
     ship->autospeed = FALSE;

#ifndef NODEATH
#ifndef NODEATHSHIP
     if ( str_cmp("Trainer", ship->owner) && str_cmp("Public",ship->owner) && ship->type != MOB_SHIP )
     {
        CLAN_DATA *clan;

        if ( ship->type != MOB_SHIP && (clan = get_clan( ship->owner )) != NULL )
        {
          if ( ship->shipclass <= SHIP_PLATFORM )
             clan->spacecraft--;
          else
             clan->vehicles--;
        }

        STRFREE( ship->owner );
        ship->owner = STRALLOC( "" );
        STRFREE( ship->pilot );
        ship->pilot = STRALLOC( "" );
        STRFREE( ship->copilot );
        ship->copilot = STRALLOC( "" );
     }
#endif
#endif
    if (!(ship->home))
    {
     if ( ship->type == SHIP_REBEL || ( ship->type == MOB_SHIP && ((!str_cmp( ship->owner , "The Rebel Alliance" )) || (!str_cmp( ship->owner , "The New Republic" )))))
     {
         STRFREE( ship->home );
         ship->home = STRALLOC( "Mon Calamari" );
     }
     else if ( ship->type == SHIP_IMPERIAL || ( ship->type == MOB_SHIP && !str_cmp(ship->owner, "the empire") ))
     {
          STRFREE( ship->home );
          ship->home = STRALLOC( "coruscant" );
     }
     else if ( ship->type == SHIP_CIVILIAN)
     {
          STRFREE( ship->home );
          ship->home = STRALLOC( "corperate" );
     }
   }

     save_ship(ship);
}

void do_resetship( CHAR_DATA *ch, char *argument )
{    
     SHIP_DATA *ship;
     
     ship = get_ship( argument );
     if (ship == NULL)
     {
        send_to_char("&RNo such ship!",ch);
        return;
     }
     
     resetship( ship ); 
     
     if ( ( ship->shipclass == SHIP_PLATFORM || ship->type == MOB_SHIP || ship->shipclass == CAPITAL_SHIP )
          && ship->home )
     {
          ship_to_spaceobject(ship, spaceobject_from_name(ship->home) );
          ship->vx = number_range( -5000 , 5000 );
          ship->vy = number_range( -5000 , 5000 );
          ship->vz = number_range( -5000 , 5000 );
	  if( ship->spaceobject )
	  {
	    ship->vx += ship->spaceobject->xpos;
	    ship->vy += ship->spaceobject->ypos;
	    ship->vz += ship->spaceobject->zpos;
	  }
          ship->shipstate = SHIP_READY;
          ship->autopilot = TRUE;
          ship->autorecharge = TRUE;
          ship->shield = ship->mod->maxshield;
     }

}

void do_setship( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    SHIP_DATA *ship;
    int  tempnum;
    ROOM_INDEX_DATA *roomindex;
    
    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    argument = one_argument( argument, arg1 );
    argument = one_argument( argument, arg2 );

    if ( arg1[0] == '\0' || arg2[0] == '\0' || arg1[0] == '\0' )
    {
	send_to_char( "Usage: setship <ship> <field> <values>\n\r", ch );
	send_to_char( "\n\rField being one of:\n\r", ch );
	send_to_char( "filename name personalname owner copilot pilot description home\n\r", ch );
	send_to_char( "cockpit entrance hanger turret cargohold\n\r", ch );
	send_to_char( "engineroom firstroom lastroom shipyard\n\r", ch );
	send_to_char( "manuever speed hyperspeed tractorbeam\n\r", ch );
	send_to_char( "lasers missiles shield hull energy chaff\n\r", ch );
	send_to_char( "comm sensor astroarray class torpedos\n\r", ch );
	send_to_char( "pilotseat coseat gunseat navseat rockets alarm\n\r", ch );
	send_to_char( "ions maxcargo cargo# dockingports guard (0-1)\n\r", ch );
	send_to_char( "\n\r\n\r", ch );
	send_to_char( "Cargo types:\n\r", ch );
	send_to_char( "Numbered 1 - 8\n\r", ch );
	send_to_char( "food metal equipment lumber medical spice\n\r", ch );
	send_to_char( "weapons valuables\n\r", ch );



	return;
    }

    ship = get_ship( arg1 );
    if ( !ship )
    {
	send_to_char( "No such ship.\n\r", ch );
	return;
    }

    if ( !str_cmp( arg2, "owner" ) )
    {
         CLAN_DATA *clan;
         if ( ship->type != MOB_SHIP && (clan = get_clan( ship->owner )) != NULL )
         {
          if ( ship->shipclass <= SHIP_PLATFORM )
             clan->spacecraft--;
          else
             clan->vehicles--;
         }
	STRFREE( ship->owner );
	ship->owner = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	if ( ship->type != MOB_SHIP && (clan = get_clan( ship->owner )) != NULL )
         {
          if ( ship->shipclass <= SHIP_PLATFORM )
             clan->spacecraft++;
          else
             clan->vehicles++;
         }
	return;
    }
    
    if ( !str_cmp( arg2, "home" ) )
    {
	STRFREE( ship->home );
	ship->home = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "pilot" ) )
    {
	STRFREE( ship->pilot );
	ship->pilot = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "copilot" ) )
    {
	STRFREE( ship->copilot );
	ship->copilot = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "firstroom" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	ship->firstroom = tempnum;
        ship->lastroom = tempnum;
        ship->cockpit = tempnum;
        ship->coseat = tempnum;
        ship->pilotseat = tempnum;
        ship->gunseat = tempnum;
        ship->navseat = tempnum;
        ship->entrance = tempnum;
        ship->hanger = 0;
	send_to_char( "You will now need to set the other rooms in the ship.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "lastroom" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom )
    	{
    	   send_to_char("The last room on a ship must be greater than or equal to the first room.\n\r",ch);
           return;
    	}
    	if ( ship->shipclass == FIGHTER_SHIP && (tempnum - ship->firstroom) > 5 )
    	{
    	   send_to_char("Starfighters may have up to 5 rooms only.\n\r",ch);
    	   return;
    	}  
	if ( ship->shipclass == MIDSIZE_SHIP && (tempnum - ship->firstroom) > 25 )
    	{
    	   send_to_char("Midships may have up to 25 rooms only.\n\r",ch);
    	   return;
    	}  
	if ( ship->shipclass == CAPITAL_SHIP && (tempnum - ship->firstroom) > 100 )
    	{
    	   send_to_char("Capital Ships may have up to 100 rooms only.\n\r",ch);
    	   return;
    	}
	ship->lastroom = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "cockpit" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->cockpit = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    
    if ( !str_cmp( arg2, "pilotseat" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	}
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->pilotseat = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    if ( !str_cmp( arg2, "coseat" ) )
    {   
        tempnum = atoi(argument);
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->coseat = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    if ( !str_cmp( arg2, "navseat" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->navseat = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    if ( !str_cmp( arg2, "gunseat" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	}
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->gunseat = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
    
    if ( !str_cmp( arg2, "entrance" ) )
    {
        tempnum = atoi(argument);
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
    	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	ship->entrance = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "turret" ) )
    {
    	char arg3[MAX_INPUT_LENGTH];
    	char arg4[MAX_INPUT_LENGTH];
    	char arg5[MAX_INPUT_LENGTH];
    	TURRET_DATA *turret;
    	int turretvnum, typeassigned;
        argument = one_argument( argument, arg3 );
        argument = one_argument( argument, arg4 );
        argument = one_argument( argument, arg5 );
        
        if ( !arg3 || arg3[0] == '\0' || !arg4 || arg4[0] == '\0' )
    	{
    	   send_to_char("Syntax: setship <ship> turret <vnum> <field> <value>\n\r",ch);
    	   send_to_char("Fields are: create delete type\n\r",ch);
           return;
    	}
    	
    	if ( !is_number(arg3) )
    	{
    	   send_to_char("Vnum must be the room that the turret is or will be located in.\n\r",ch);
           return;
    	}
    	
    	turretvnum = atoi(arg3);
    	
    	if ( !str_cmp( arg4, "create" ) )
    	{
//    	   send_to_char("Not quite done with this yet-DV.\n\r",ch);
//           return;
//    	}
	   CREATE( turret, TURRET_DATA, 1 );
	   ship->shipclass == MIDSIZE_SHIP ? (turret->type = QUAD_TURRET) : (turret->type = TURBOLASER_TURRET);
	   turret->roomvnum	= turretvnum;
	   turret->state	= LASER_READY;

	   LINK( turret, ship->first_turret, ship->last_turret, next, prev );
	   save_ship(ship);
	   return;
	}
    	if ( !str_cmp( arg4, "delete" ) )
    	{
//    	   send_to_char("Not quite done with this yet-DV.\n\r",ch);
//	   return;
//    	}
    	   if( !ship->first_turret )
    	   {
    	     send_to_char("That ship has no turrets.\n\r",ch);
             return;
    	   }
    	   for ( turret = ship->first_turret; turret; turret = turret->next )
    	   {
    	   	if ( turret->roomvnum == turretvnum )
    	   	  break;
    	   }
    	   if ( !turret )
    	   {
    	     send_to_char("There is no turret with that vnum on that ship.\n\r",ch);
             return;
    	   }
	   turret->type	 	= 0;
	   turret->roomvnum	= 0;
	   turret->state	= 0;

	   UNLINK( turret, ship->first_turret, ship->last_turret, next, prev );
	   DISPOSE(turret);
	   save_ship(ship);
	   return;
	}

    	if ( !str_cmp( arg4, "type" ) )
    	{
    	   if( !arg5 || arg5[0] == '\0' || !is_number(arg5) )
    	   {
    	     send_to_char("Syntax: setship <ship> turret <vnum> type <type number>.\n\r",ch);
             return;
    	   }
    	   typeassigned = atoi(arg5);
    	   if ( typeassigned > MAX_TURRET_TYPE )
    	   {
    	     int x;
    	     send_to_char("Turret type invalid. Valid types follow.\n\r",ch);
    	     for ( x = 0; x < MAX_TURRET_TYPE; x++ )
    	     {
    	       send_to_char( turretstats[x].name, ch );
    	       send_to_char( "\n\r", ch );
    	     }
    	     return;
    	   }
    	   if( !ship->first_turret )
    	   {
    	     send_to_char("That ship has no turrets.\n\r",ch);
             return;
    	   }
    	   for ( turret = ship->first_turret; turret; turret = turret->next )
    	   {
    	   	if ( turret->roomvnum == turretvnum )
    	   	  break;
    	   }
    	   if ( !turret )
    	   {
    	     send_to_char("There is no turret with that vnum on that ship.\n\r",ch);
             return;
    	   }
    	   turret->type = typeassigned;
    	   send_to_char("Done.\n\r", ch);
    	   save_ship(ship);
    	   return;
    	   
	}          
    }


/*  if ( !str_cmp( arg2, "turret1" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
    	if ( ship->shipclass == FIGHTER_SHIP )
    	{
    	   send_to_char("Starfighters can't have extra laser turrets.\n\r",ch);
    	   return;
    	}
	if ( tempnum == ship->cockpit || tempnum == ship->entrance ||
    	     tempnum == ship->turret2 || tempnum == ship->hanger || tempnum == ship->engineroom )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->turret1 = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "turret2" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
	if ( ship->shipclass == FIGHTER_SHIP )
    	{
    	   send_to_char("Starfighters can't have extra laser turrets.\n\r",ch);
    	   return;
    	}
	if ( tempnum == ship->cockpit || tempnum == ship->entrance ||
    	     tempnum == ship->turret1 || tempnum == ship->hanger || tempnum == ship->engineroom )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->turret2 = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
*/
    if ( !str_cmp( arg2, "hanger" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL && atoi(argument) != 0 )
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	if (( tempnum < ship->firstroom || tempnum > ship->lastroom ) && (atoi(argument) != 0 ))
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
    	if ( tempnum == ship->cockpit || tempnum == ship->entrance || tempnum == ship->engineroom )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	if ( ship->shipclass == FIGHTER_SHIP )
	{
	   send_to_char("Starfighters are to small to have hangers for other ships!\n\r",ch);
	   return;
	}
	ship->hanger = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
/*
    if ( !str_cmp( arg2, "cargohold" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
    	if ( tempnum == ship->cockpit || tempnum == ship->entrance || tempnum == ship->engineroom )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	if ( ship->shipclass == FIGHTER_SHIP )
	{
	   send_to_char("Starfighters are to small to have cargo holds!\n\r",ch);
	   return;
	}
	ship->cargohold = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
*/
   if ( !str_cmp( arg2, "engineroom" ) )
    {   
        tempnum = atoi(argument); 
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.\n\r",ch);
    	   return;
    	} 
	if ( tempnum < ship->firstroom || tempnum > ship->lastroom )
    	{
    	   send_to_char("That room number is not in that ship .. \n\rIt must be between Firstroom and Lastroom.\n\r",ch);
           return;
    	}
    	if ( tempnum == ship->cockpit || tempnum == ship->entrance || tempnum == ship->hanger )
    	{
    	   send_to_char("That room is already being used by another part of the ship\n\r",ch);
           return;
    	}
	ship->engineroom = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "shipyard" ) )
    {
        tempnum = atoi(argument);
    	roomindex = get_room_index(tempnum);
    	if (roomindex == NULL)
    	{
    	   send_to_char("That room doesn't exist.",ch);
    	   return;
    	} 
	ship->shipyard = tempnum;
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "type" ) )
    {
	if ( !str_cmp( argument, "rebel" ) )
	  ship->type = SHIP_REBEL;
	else
	if ( !str_cmp( argument, "imperial" ) )
	  ship->type = SHIP_IMPERIAL;
	else
	if ( !str_cmp( argument, "civilian" ) )
	  ship->type = SHIP_CIVILIAN;
	else
	if ( !str_cmp( argument, "mob" ) )
	  ship->type = MOB_SHIP;
	else
	{
	   send_to_char( "Ship type must be either: rebel, imperial, civilian or mob.\n\r", ch );
	   return;
	}
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "name" ) )
    {
	STRFREE( ship->name );
	ship->name = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "personalname" ) )
    {
      if ( ship->personalname )
	STRFREE( ship->personalname );
      ship->personalname = STRALLOC( argument );
      send_to_char( "Done.\n\r", ch );
      save_ship( ship ); 
      return;
    }

    if ( !str_cmp( arg2, "filename" ) )
    {
	DISPOSE( ship->filename );
	ship->filename = str_dup( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	write_ship_list( );
	return;
    }

    if ( !str_cmp( arg2, "desc" ) )
    {
	STRFREE( ship->description );
	ship->description = STRALLOC( argument );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "dockingports" ) )
    {   
	ship->dockingports = URANGE( -1, atoi(argument) , 20 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "guard" ) )
    {   
	ship->guard = URANGE( -1, atoi(argument) , 1 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "manuever" ) )
    {   
	ship->manuever = URANGE( 0, atoi(argument) , 250 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "upgradeblock" ) )
    {   
        if ( ch->top_level < LEVEL_SUPREME )
          return;
	ship->upgradeblock = atoi(argument);
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "lasers" ) )
    {
   if ( ch->top_level == LEVEL_SUPREME )
     ship->lasers = URANGE( 0, atoi(argument) , 20 );
   else
	  ship->lasers = URANGE( 0, atoi(argument) , 10 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "ions" ) )
    {
   if ( ch->top_level == LEVEL_SUPREME )
     ship->ions = URANGE( 0, atoi(argument) , 20 );
   else
	  ship->ions = URANGE( 0, atoi(argument) , 10 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "gravitypower" ) )
    {
        ship->mod->gravitypower = atoi(argument);
      save_ship( ship );
      return;
    }

    if ( !str_cmp( arg2, "gravproj" ) )
    {
        ship->mod->gravproj= atoi(argument);
      save_ship( ship );
      return;
    }

    if ( !str_cmp( arg2, "maxcargo" ) )
    {
   if ( ch->top_level == LEVEL_SUPREME )
     ship->maxcargo = URANGE( 0, atoi(argument) , 30000 );
   else
	  ship->maxcargo = URANGE( 0, atoi(argument) , 1000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

/*  if ( !str_cmp( arg2, "cargo1" ) )
    {
   if ( ch->top_level == LEVEL_SUPREME )
     ship->cargo1 = URANGE( 0, atoi(argument) , 30000 );
   else
	  ship->cargo1 = URANGE( 0, atoi(argument) , ship->maxcargo );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
*/

    if ( !str_cmp( arg2, "class" ) )
    {
	ship->shipclass = URANGE( 0, atoi(argument) , 9 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "missiles" ) )
    {
	ship->missiles = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "torpedos" ) )
    {
	ship->torpedos = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "rockets" ) )
    {
	ship->rockets = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "speed" ) )
    {
   if ( ch->top_level == LEVEL_SUPREME )
     ship->realspeed = URANGE( 0, atoi(argument) , 255 );
   else
	  ship->realspeed = URANGE( 0, atoi(argument) , 150 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "tractorbeam" ) )
    {
	ship->tractorbeam = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }
 
    if ( !str_cmp( arg2, "hyperspeed" ) )
    {
	ship->hyperspeed = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "shield" ) )
    {
   if ( ch->top_level == LEVEL_SUPREME )
     ship->maxshield = URANGE( 0, atoi(argument) , 30000 );
   else
	  ship->maxshield = URANGE( 0, atoi(argument) , 1000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "hull" ) )
    {
   if ( ch->top_level == LEVEL_SUPREME )
	{ ship->hull = URANGE( 1, atoi(argument) , 30000 );
     ship->maxhull = URANGE( 1, atoi(argument) , 30000 );
   }
   else {
	  ship->hull = URANGE( 1, atoi(argument) , 20000 );
	  ship->maxhull = URANGE( 1, atoi(argument) , 20000 );
   }
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "energy" ) )
    {
	ship->energy = URANGE( 1, atoi(argument) , 30000 );
	ship->maxenergy = URANGE( 1, atoi(argument) , 30000 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "sensor" ) )
    {
	ship->sensor = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "astroarray" ) )
    {
	ship->astro_array = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "comm" ) )
    {
	ship->comm = URANGE( 0, atoi(argument) , 255 );
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp( arg2, "chaff" ) )
    {
   if ( ch->top_level == LEVEL_SUPREME )
   { ship->chaff = URANGE( 0, atoi(argument) , 255 );
   }
   else {
	  ship->chaff = URANGE( 0, atoi(argument) , 25 );
   }
	send_to_char( "Done.\n\r", ch );
	save_ship( ship );
	return;
    }

    if ( !str_cmp(arg2,"alarm") )
    {
        ship->alarm = URANGE(0,atoi(argument),5);
        send_to_char("Done.\n\r",ch);
        save_ship(ship);
        return;
    }

    if ( !str_cmp( arg2, "maxintmodules" ) )
	{
		ship->maxintmodules = atoi(argument);
		return;
	}
    if ( !str_cmp( arg2, "maxextmodules" ) )
	{
		ship->maxextmodules = atoi(argument);
		return;
	}

    do_setship( ch, "" );
    return;
}

void do_showship( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    TURRET_DATA *turret;
    sh_int turretnum = 0;

    if ( IS_NPC( ch ) )
    {
	send_to_char( "Huh?\n\r", ch );
	return;
    }

    if ( argument[0] == '\0' )
    {
	send_to_char( "Usage: showship <ship>\n\r", ch );
	return;
    }

    ship = get_ship( argument );
    if ( !ship )
    {
	send_to_char( "No such ship.\n\r", ch );
	return;
    }
    set_char_color( AT_YELLOW, ch );
    ch_printf( ch, "%s %s : %s (%s)\n\rFilename: %s ShipID: %ld\n\r",
			ship->type == SHIP_REBEL ? "Rebel Alliance" :
		       (ship->type == SHIP_IMPERIAL ? "Imperial" : 
		       (ship->type == SHIP_CIVILIAN ? "Civilian" : "Mob" ) ),
		        ship->shipclass == FIGHTER_SHIP ? "Starfighter" :
		       (ship->shipclass == MIDSIZE_SHIP ? "Midship" : 
		       (ship->shipclass == CAPITAL_SHIP ? "Capital Ship" : 
		       (ship->shipclass == SHIP_PLATFORM ? "Platform" : 
		       (ship->shipclass == CLOUD_CAR ? "Cloudcar" : 
		       (ship->shipclass == OCEAN_SHIP ? "Boat" :
		       (ship->shipclass == LAND_SPEEDER ? "Speeder" :
		       (ship->shipclass == WHEELED ? "Wheeled Transport" : 
		       (ship->shipclass == LAND_CRAWLER ? "Crawler" : 
		       (ship->shipclass == WALKER ? "Walker" : "Unknown" ) ) ) ) ) ) ) ) ), 
    			ship->name, 
    			ship->personalname,
    			ship->filename,
    			ship->shipID);
    ch_printf( ch, "Home: %s   Description: %s\n\rOwner: %s   Pilot: %s   Copilot: %s\n\r",
    			ship->home,  ship->description,
    			ship->owner, ship->pilot,  ship->copilot );
    ch_printf( ch, "Current Jump Destination: %s  Jump Point: %s\n\r", (ship->currjump ? ship->currjump->name : "(null)"), (ship->lastsystem ? ship->lastsystem->name : "(null)" ));
    ch_printf( ch, "Firstroom: %d   Lastroom: %d",
    			ship->firstroom,
    			ship->lastroom);
    ch_printf( ch, "Cockpit: %d   Entrance: %d   Hanger: %d  Engineroom: %d\n\r",
    			ship->cockpit,
    			ship->entrance,
    			ship->hanger,
    			ship->engineroom);
    ch_printf( ch, "Pilotseat: %d   Coseat: %d   Navseat: %d  Gunseat: %d\n\r",
    			ship->pilotseat,
    			ship->coseat,
    			ship->navseat,
    			ship->gunseat);
    ch_printf( ch, "Location: %d   Lastdoc: %d   Shipyard: %d\n\r",
    			ship->location,
    			ship->lastdoc,
    			ship->shipyard);
    ch_printf( ch, "Tractor Beam: %d   Comm: %d   Sensor: %d   Astro Array: %d\n\r",
    			ship->mod->tractorbeam,
    			ship->mod->comm,
    			ship->mod->sensor,
    			ship->mod->astro_array);
    ch_printf( ch, "Lasers: %d  Ions: %d   Laser Condition: %s\n\r",
    			ship->mod->lasers, ship->mod->ions,
    			ship->statet0 == LASER_DAMAGED ? "Damaged" : "Good");

   if( ship->first_turret )
     for( turret = ship->first_turret, turretnum = 1; turret; turret = turret->next, turretnum++ )
       ch_printf( ch, "Turret%d Type: %d Vnum: %d State: %s\n\r",
         turretnum,
         turret->type, 
         turret->roomvnum,
         turret->state == LASER_DAMAGED ? "Damaged" : "Good");

    ch_printf( ch, "Missiles: %d  Torpedos: %d  Rockets: %d  Launchers: %dCondition: %s\n\r",
       			ship->missiles,
    			ship->torpedos,
    			ship->rockets,
    			ship->mod->launchers,
    			ship->missilestate == MISSILE_DAMAGED ? "Damaged" : "Good");		
    ch_printf( ch, "Hull: %d/%d  Ship Condition: %s\n\r",
                        ship->hull,
    		        ship->mod->maxhull,
    			ship->shipstate == SHIP_DISABLED ? "Disabled" : "Running");
    		
    ch_printf( ch, "Shields: %d/%d   Energy(fuel): %d/%d   Chaff: %d Launchers: %d\n\r",
                        ship->shield,
    		        ship->mod->maxshield,
    		        ship->energy,
    		        ship->mod->maxenergy,
    		        ship->chaff,
    		        ship->mod->defenselaunchers);
    if( ship->mod )
      ch_printf (ch, "Gravity Projector: %d/%d\n\r",
        ship->mod->gravproj,
        ship->mod->gravitypower );
    else
      ch_printf (ch, "Gravity Projector: (NULL)\n\r");
        		        
    ch_printf( ch, "Current Coordinates: %.0f %.0f %.0f\n\r",
                        ship->vx, ship->vy, ship->vz );
    ch_printf( ch, "Current Heading: %.0f %.0f %.0f\n\r",
                        ship->hx, ship->hy, ship->hz );
    ch_printf( ch, "Speed: %d/%d   Hyperspeed: %d   Manueverability: %d\r\n",
                        ship->currspeed, ship->mod->realspeed, ship->mod->hyperspeed , ship->mod->manuever );                    
    ch_printf( ch, "Docked: ");
if ((ship->docked) != NULL)
	{
	ch_printf( ch, "with %s",ship->docked->name);
	}
	else
	{
	ch_printf( ch, "NO");
	}
	ch_printf(ch, "  Docking Ports: %d", ship->dockingports );
        ch_printf(ch,"  Alarm: %d   ",ship->alarm);
        
        if(ship->upgradeblock)
          ch_printf( ch, "UpgradeBlock: %d\n\r", ship->upgradeblock );
        else
          ch_printf( ch, "UpgradeBlock: NO\n\r" );
        

  ch_printf( ch, "Max. Modules Ext: %d Int:%d\n\r", ship->maxextmodules, ship->maxintmodules);

   ch_printf(ch, "Maxcargo: %d  Current Cargo: %d\n\r", ship->maxcargo, 0 );
    return;
}

void do_makeship( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    char arg[MAX_INPUT_LENGTH];
    
    argument = one_argument( argument, arg );
    
    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: makeship <filename> <ship name>\n\r", ch );
	return;
    }

    CREATE( ship, SHIP_DATA, 1 );
    LINK( ship, first_ship, last_ship, next, prev );

    ship->name		= STRALLOC( argument );
    ship->personalname		= STRALLOC( argument );
    ship->description	= STRALLOC( "" );
    ship->owner 	= STRALLOC( "" );
    ship->copilot       = STRALLOC( "" );
    ship->pilot         = STRALLOC( "" );
    ship->home          = STRALLOC( "" );
    ship->type          = SHIP_CIVILIAN;
    ship->spaceobject = NULL;
    ship->energy = ship->maxenergy;
    ship->hull = ship->maxhull;
    ship->in_room=NULL;
    ship->next_in_room=NULL;
    ship->prev_in_room=NULL;
    ship->currjump=NULL;
    ship->target0=NULL;
    ship->maxextmodules=10;
    ship->maxintmodules=3;
    ship->modules=0;

    ship->filename = str_dup( arg );
    save_ship( ship );
    write_ship_list( );
	
}

void do_copyship( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    SHIP_DATA *old;
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];

    argument = one_argument( argument, arg );
    argument = one_argument( argument, arg2 );
    
    if ( !argument || argument[0] == '\0' )
    {
	send_to_char( "Usage: copyship <oldshipname> <filename> <newshipname>\n\r", ch );
	return;
    }

    old = get_ship ( arg );
    
    if (!old)
    {
	send_to_char( "Thats not a ship!\n\r", ch );
	return;
    }

    CREATE( ship, SHIP_DATA, 1 );
    LINK( ship, first_ship, last_ship, next, prev );

    ship->name		= STRALLOC( argument );
    ship->description	= STRALLOC( "" );
    ship->owner 	= STRALLOC( "" );
    ship->copilot       = STRALLOC( "" );
    ship->pilot         = STRALLOC( "" );
    ship->home          = STRALLOC( "" );
    ship->type          = old->type;
    ship->shipclass         = old->shipclass;
    ship->lasers        = old->lasers  ;
    ship->maxshield        = old->maxshield  ;
    ship->maxhull        = old->maxhull  ;
    ship->maxenergy        = old->maxenergy  ;
    ship->hyperspeed        = old->hyperspeed  ;
    ship->realspeed        = old->realspeed  ;
    ship->manuever        = old->manuever  ;
    ship->in_room=NULL;
    ship->next_in_room=NULL;
    ship->prev_in_room=NULL;
    ship->currjump=NULL;
    ship->target0=NULL;

    ship->filename         = str_dup(arg2);
    save_ship( ship );
    write_ship_list();
}

void do_ships( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    char buf[MAX_STRING_LENGTH];
    char pilottype[MAX_STRING_LENGTH];
    char pilottype2[MAX_STRING_LENGTH];
    int count;
    bool owned, set;
    
    if ( !IS_NPC(ch) )
    {
      count = 0;
      send_to_pager( "&YThe following ships you have pilot access to:\n\r", ch );
      send_to_pager( "\n\r&WShip                                                   Owner\n\r",ch);
      for ( ship = first_ship; ship; ship = ship->next )
      {
      	owned = FALSE, set = FALSE;
        if ( str_cmp(ship->owner, ch->name) )
        {
           if ( !check_pilot( ch, ship ) || !str_cmp(ship->owner, "public") || !str_cmp(ship->owner, "trainer") )
               continue;
        }
        
        if( ship->shipclass > SHIP_PLATFORM )
          continue;

        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REBEL)
           set_pager_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_pager_color( AT_DGREEN, ch );
        else
          set_pager_color( AT_BLUE, ch );

       if( !str_cmp(ship->owner, ch->name ) )
       {
         strcpy( pilottype2, "Owner" );
         owned = TRUE;
         set = TRUE;
       }
       if( !set && !str_cmp( ship->pilot, ch->name ) )
       {
         strcpy( pilottype2, "Pilot" );
         set = TRUE;
       }
       if( !set && !str_cmp( ship->pilot, ch->name ) )
       {
         strcpy( pilottype2, "Co-Pilot" );
         set = TRUE;
       }
       if( !set )
       {
         strcpy( pilottype2, "Clan-Pilot" );
         set = TRUE;
       }

       if( !owned )
         sprintf( pilottype, "(%s) - %s", pilottype2, ship->owner );
       else
         sprintf( pilottype, "(%s)", pilottype2 );
	
       sprintf( buf, "%s (%s)", ship->name, ship->personalname );

        if  ( ship->in_room )
          pager_printf( ch, "%-35s (%s) \n&R&W- %-24s&R&w \n\r", buf, ship->in_room->name, pilottype );
        else
          pager_printf( ch, "%-35s (%.0f %.0f %.0f) \n&R&W- %-35s&R&w\n\r", buf, ship->vx, ship->vy, ship->vz, pilottype );

        count++;
      }

      if ( !count )
      {
        send_to_pager( "There are no ships owned by you.\n\r", ch );
      }

    }


    count =0;
    send_to_pager( "&Y\n\rThe following ships are docked here:\n\r", ch );

    send_to_pager( "\n\r&WShip                               Owner          Cost/Rent\n\r", ch );
    for ( ship = first_ship; ship; ship = ship->next )
    {   
        if ( ship->location != ch->in_room->vnum || ship->shipclass > SHIP_PLATFORM)
               continue;

        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REBEL)
           set_pager_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_pager_color( AT_DGREEN, ch );
        else
          set_pager_color( AT_BLUE, ch );

       sprintf( buf, "%s (%s)", ship->name, ship->personalname );
        
        pager_printf( ch, "%-35s %-15s", buf, ship->owner );
        if (ship->type == MOB_SHIP || ship->shipclass == SHIP_PLATFORM )
        {
          pager_printf( ch, "\n\r");
          continue;
        }
        if ( !str_cmp(ship->owner, "Public") )
        { 
          pager_printf( ch, "%ld to rent.\n\r", get_ship_value(ship)/100 ); 
        }
        else if ( str_cmp(ship->owner, "") )
          pager_printf( ch, "%s", "\n\r" );
        else
           pager_printf( ch, "%ld to buy.\n\r", get_ship_value(ship) ); 
        
        count++;
    }

    if ( !count )
    {
        send_to_pager( "There are no ships docked here.\n\r", ch );
    }
}

void do_speeders( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int count;
    char buf[MAX_STRING_LENGTH];
    char pilottype[MAX_STRING_LENGTH];
    char pilottype2[MAX_STRING_LENGTH];
    bool owned, set;

    if ( !IS_NPC(ch) )
    {
      count = 0;
      send_to_pager( "&YThe following ships you have pilot access to:\n\r", ch );
      send_to_pager( "\n\r&WShip                                                   Owner\n\r",ch);
      for ( ship = first_ship; ship; ship = ship->next )
      {
      	owned = FALSE, set = FALSE;
        if ( str_cmp(ship->owner, ch->name) )
        {
           if ( !check_pilot( ch, ship ) || !str_cmp(ship->owner, "public") || !str_cmp(ship->owner, "trainer") )
               continue;
        }
        
        if( ship->shipclass <= SHIP_PLATFORM )
          continue;

        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REBEL)
           set_pager_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_pager_color( AT_DGREEN, ch );
        else
          set_pager_color( AT_BLUE, ch );

       if( !str_cmp(ship->owner, ch->name ) )
       {
         strcpy( pilottype2, "Owner" );
         owned = TRUE;
         set = TRUE;
       }
       if( !set && !str_cmp( ship->pilot, ch->name ) )
       {
         strcpy( pilottype2, "Pilot" );
         set = TRUE;
       }
       if( !set && !str_cmp( ship->pilot, ch->name ) )
       {
         strcpy( pilottype2, "Co-Pilot" );
         set = TRUE;
       }
       if( !set )
       {
         strcpy( pilottype2, "Clan-Pilot" );
         set = TRUE;
       }

       if( !owned )
         sprintf( pilottype, "(%s) - %s", pilottype2, ship->owner );
       else
         sprintf( pilottype, "(%s)", pilottype2 );
	
       sprintf( buf, "%s (%s)", ship->name, ship->personalname );

        if  ( ship->in_room )
          pager_printf( ch, "%-35s (%s) \n&R&W- %-24s&R&w \n\r", buf, ship->in_room->name, pilottype );
        else
          pager_printf( ch, "%-35s (%.0f %.0f %.0f) \n&R&W- %-35s&R&w\n\r", buf, ship->vx, ship->vy, ship->vz, pilottype );

        count++;
      }

      if ( !count )
      {
        send_to_pager( "There are no ships owned by you.\n\r", ch );
      }

    }


    
    count =0;
    send_to_pager( "&Y\n\rThe following vehicles are parked here:\n\r", ch );
    
    send_to_pager( "\n\r&WVehicle                            Owner          Cost/Rent\n\r", ch );
    for ( ship = first_ship; ship; ship = ship->next )
    {   
        if ( ship->location != ch->in_room->vnum || ship->shipclass <= SHIP_PLATFORM)
               continue;
               
        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REBEL)
           set_pager_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_pager_color( AT_DGREEN, ch );
        else
          set_pager_color( AT_BLUE, ch );
        

        sprintf( buf, "%s(%s)", ship->name, ship->personalname );
        pager_printf( ch, "%-35s%-15s ", buf, ship->owner );
        
        if ( !str_cmp(ship->owner, "Public") )
        { 
          pager_printf( ch, "%ld to rent.\n\r", get_ship_value(ship)/100 ); 
        }
        else if ( str_cmp(ship->owner, "") )
          pager_printf( ch, "%s", "\n\r" );
        else
           pager_printf( ch, "%ld to buy.\n\r", get_ship_value(ship) ); 
        
        count++;
    }

    if ( !count )
    {
        send_to_pager( "There are no sea air or land vehicles here.\n\r", ch );
    }
}

void do_allspeeders( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int count = 0;
    char buf[MAX_STRING_LENGTH];

      count = 0;
      send_to_pager( "&Y\n\rThe following sea/land/air vehicles are currently formed:\n\r", ch );
    
      send_to_pager( "\n\r&WVehicle                            Owner\n\r", ch );
      for ( ship = first_ship; ship; ship = ship->next )
      {   
        if ( ship->shipclass <= SHIP_PLATFORM ) 
           continue; 
      
        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REBEL)
           set_pager_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_pager_color( AT_DGREEN, ch );
        else
          set_pager_color( AT_BLUE, ch );
        
        sprintf( buf, "%s(%s)", ship->name, ship->personalname );
        pager_printf( ch, "%-35s%-15s ", buf, ship->owner );

        if ( !str_cmp(ship->owner, "Public") )
        { 
          pager_printf( ch, "%ld to rent.\n\r", get_ship_value(ship)/100 ); 
        }
        else if ( str_cmp(ship->owner, "") )
          pager_printf( ch, "%s", "\n\r" );
        else
           pager_printf( ch, "%ld to buy.\n\r", get_ship_value(ship) );
        
        count++;
      }
    
      if ( !count )
      {
        send_to_pager( "There are none currently formed.\n\r", ch );
	return;
      }
    
}

void do_allships( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    char buf[MAX_STRING_LENGTH];
    int count = 0;
    bool unowned = FALSE, mobship = FALSE, checkowner = FALSE, data = FALSE;
    int type = -1;
 
    if ( !str_cmp( argument, "unowned" ) )
      unowned = TRUE;
    else if ( !str_cmp( argument, "imperial" ) )
      type = SHIP_IMPERIAL;
    else if ( !str_cmp( argument, "rebel" ) )
      type = SHIP_REBEL;
    else if ( !str_cmp( argument, "civilian" ) )
      type = SHIP_CIVILIAN;
    else if ( !str_cmp( argument, "mob" ) )
      mobship = TRUE;
    else if (!str_cmp (argument, "data"))
      data = TRUE;
    else if (!argument || argument[0] == '\0' || !str_cmp (argument, ""))
      ;
    else
      checkowner = TRUE;

    if ( data )
    {
      send_to_pager ( "\n\rPersonal name            Filename       Owner     \n\r", ch);
      for ( ship = first_ship; ship; ship = ship->next )
        pager_printf( ch, "%25s | %15s | %10s\n\r", ship->personalname, ship->filename, ship->owner );
      return;
    }
      
      count = 0;
      send_to_pager( "&Y\n\rThe following ships are currently formed:\n\r", ch );
    
      send_to_pager( "\n\r&WShip                               Owner\n\r", ch );
      
      if ( IS_IMMORTAL( ch ) && !unowned && !checkowner && type < 0)
        for ( ship = first_ship; ship; ship = ship->next )
           if (ship->type == MOB_SHIP && ship->shipclass != SHIP_DEBRIS )
           {
              sprintf( buf, "%s(%s)", ship->name, ship->personalname );
              pager_printf( ch, "&w%-35s %-10s\n\r", buf, ship->owner );
           }
     if( !mobship )       
      for ( ship = first_ship; ship; ship = ship->next )
      {   
        if ( ship->shipclass > SHIP_PLATFORM ) 
           continue; 
      
        if( unowned && str_cmp(ship->owner, "") )
          continue;
        if( checkowner && str_cmp(ship->owner, argument) )
          continue;
          
        if( type >= 0 && ship->type != type )
          continue;  
      
        if (ship->type == MOB_SHIP)
           continue;
        else if (ship->type == SHIP_REBEL)
           set_pager_color( AT_BLOOD, ch );
        else if (ship->type == SHIP_IMPERIAL)
           set_pager_color( AT_DGREEN, ch );
        else
          set_pager_color( AT_BLUE, ch );
        sprintf( buf, "%-10s(%-10s)", ship->name, ship->personalname );
        pager_printf( ch, "&w%-35s %-15s\n\r", buf, ship->owner );
        if (ship->type == MOB_SHIP || ship->shipclass == SHIP_PLATFORM )
        {
          pager_printf( ch, "\n\r");
          continue;
        }
        if ( !str_cmp(ship->owner, "Public") )
        { 
          pager_printf( ch, "%ld to rent.\n\r", get_ship_value(ship)/100 ); 
        }
        else if ( str_cmp(ship->owner, "") )
          pager_printf( ch, "%s", "\n\r" );
        else
           pager_printf( ch, "&W%-10s%ld to buy.&R&w\n\r", "", get_ship_value(ship) ); 
        
        count++;
      }
    
      if ( !count )
      {
        send_to_pager( "There are no ships currently formed.\n\r", ch );
	return;
      }
    
}


void ship_to_spaceobject( SHIP_DATA *ship , SPACE_DATA *spaceobject )
{
     if ( spaceobject == NULL )
        return;
     
     if ( ship == NULL )
        return;
     
     ship->spaceobject = spaceobject;
        
}

void new_missile( SHIP_DATA *ship , SHIP_DATA *target , CHAR_DATA *ch , int missiletype )
{
     SPACE_DATA *spaceobject;
     MISSILE_DATA *missile;

     if ( ship  == NULL )
        return;

     if ( target  == NULL )
        return;

     if ( ( spaceobject = ship->spaceobject ) == NULL )
        return;
          
     CREATE( missile, MISSILE_DATA, 1 );
     LINK( missile, first_missile, last_missile, next, prev );

     missile->target = target; 
     missile->fired_from = ship;
     if ( ch )
        missile->fired_by = STRALLOC( ch->name );
     else 
        missile->fired_by = STRALLOC( "" );
     missile->missiletype = missiletype;
     missile->age =0;
     if ( missile->missiletype == HEAVY_BOMB )
       missile->speed = 20;
     else if ( missile->missiletype == PROTON_TORPEDO ) 
       missile->speed = 200;
     else if ( missile->missiletype == CONCUSSION_MISSILE ) 
       missile->speed = 300;
     else 
       missile->speed = 50;
     
     missile->mx = ship->vx;
     missile->my = ship->vy;
     missile->mz = ship->vz;
            
     missile->spaceobject = spaceobject;
        
}

void ship_from_spaceobject( SHIP_DATA *ship , SPACE_DATA *spaceobject )
{

     if ( spaceobject == NULL )
        return;

     if ( ship == NULL )
        return;
     
     ship->spaceobject = NULL;

}

void extract_missile( MISSILE_DATA *missile )
{
    SPACE_DATA *spaceobject;

     if ( missile == NULL )
        return;

     if ( ( spaceobject = missile->spaceobject ) != NULL )
      missile->spaceobject = NULL;
     
     UNLINK( missile, first_missile, last_missile, next, prev );
     
     missile->target = NULL; 
     missile->fired_from = NULL;
     if (  missile->fired_by )
        STRFREE( missile->fired_by );
     
     DISPOSE( missile );
          
}

bool is_rental( CHAR_DATA *ch , SHIP_DATA *ship )
{
   if ( !str_cmp("Public",ship->owner) )
          return TRUE;
   if ( !str_cmp("Trainer",ship->owner) )
          return TRUE;
   if ( ship->shipclass == SHIP_TRAINER )
          return TRUE;

   return FALSE;
}

bool check_pilot( CHAR_DATA *ch , SHIP_DATA *ship )
{
   if ( !str_cmp(ch->name,ship->owner) || !str_cmp(ch->name,ship->pilot)
   || !str_cmp(ch->name,ship->copilot) || !str_cmp("Public",ship->owner)
   || !str_cmp("Trainer", ship->owner) )
      return TRUE;

   if ( !IS_NPC(ch) && ch->pcdata && ch->pcdata->clan )
   {
      if ( !str_cmp(ch->pcdata->clan->name,ship->owner) )
      {
        if ( !str_cmp(ch->pcdata->clan->leader,ch->name) )
          return TRUE;
        if ( !str_cmp(ch->pcdata->clan->number1,ch->name) )
          return TRUE;
        if ( !str_cmp(ch->pcdata->clan->number2,ch->name) )
          return TRUE;
        if ( ch->pcdata->bestowments && is_name( "pilot", ch->pcdata->bestowments) )
          return TRUE;
      }
      if ( !str_cmp(ch->pcdata->clan->name,ship->pilot) )
      {
        if ( !str_cmp(ch->pcdata->clan->leader,ch->name) )
          return TRUE;
        if ( !str_cmp(ch->pcdata->clan->number1,ch->name) )
          return TRUE;
        if ( !str_cmp(ch->pcdata->clan->number2,ch->name) )
          return TRUE;
        if ( ch->pcdata->bestowments && is_name( "pilot", ch->pcdata->bestowments) )
          return TRUE;
      }
      if ( !str_cmp(ch->pcdata->clan->name,ship->copilot) )
      {
        if ( !str_cmp(ch->pcdata->clan->leader,ch->name) )
          return TRUE;
        if ( !str_cmp(ch->pcdata->clan->number1,ch->name) )
          return TRUE;
        if ( !str_cmp(ch->pcdata->clan->number2,ch->name) )
          return TRUE;
        if ( ch->pcdata->bestowments && is_name( "pilot", ch->pcdata->bestowments) )
          return TRUE;
      }


   }

   return FALSE;
}

bool extract_ship( SHIP_DATA *ship )
{   
    ROOM_INDEX_DATA *room;
    
    if ( ( room = ship->in_room ) != NULL )
    {
        UNLINK( ship, room->first_ship, room->last_ship, next_in_room, prev_in_room );
        ship->in_room = NULL;
    }
    return TRUE;
}

bool damage_ship_ch( SHIP_DATA *ship , int min , int max , CHAR_DATA *ch )
{
    sh_int ionFactor = 1;
    int damage , shield_dmg;
    long xp;
    bool ions = FALSE;
    char  logbuf[MAX_STRING_LENGTH];
    char  buf[MAX_STRING_LENGTH];
    TURRET_DATA *turret;
    int turretnum;

    if ( min < 0 && max < 0 )
    {
      ions = TRUE;
      damage = number_range( max*(-1), min*(-1) );
    }
    else
      damage = number_range( min , max );

    if ( ions == TRUE )
      ionFactor = 2;

    xp = ( exp_level( ch->skill_level[PILOTING_ABILITY]+1) - exp_level( ch->skill_level[PILOTING_ABILITY]) ) / 25 ;
    xp = UMIN( get_ship_value( ship ) /100 , xp ) ;
    gain_exp( ch , xp , PILOTING_ABILITY );

    if ( ship->shield > 0 )
    {
      shield_dmg = UMIN( ship->shield , damage );
 	   damage -= shield_dmg;
  	   ship->shield -= shield_dmg;
    	if ( ship->shield == 0 )
    	  echo_to_cockpit( AT_BLOOD , ship , "Shields down..." );
    }

    if ( damage > 0 )
    {
      if ( number_range(1, 100) <= 5*ionFactor && ship->shipstate != SHIP_DISABLED )
      {
         echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!" );
         ship->shipstate = SHIP_DISABLED;
      }

      if ( number_range(1, 100) <= 5*ionFactor && ship->missilestate != MISSILE_DAMAGED && ship->mod->launchers > 0 )
      {
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Missile Launcher DAMAGED!" );
         ship->missilestate = MISSILE_DAMAGED;
      }
      if ( number_range(1, 100) <= 2*ionFactor && ship->statet0 != LASER_DAMAGED )
      {
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Lasers DAMAGED!" );
         ship->statet0 = LASER_DAMAGED;
      }
      if ( ship->first_turret )
        for( turret = ship->first_turret, turretnum = 0; turret; turret = turret->next, turretnum++ )
          if ( number_range(1, 100) <= 5*ionFactor && turret->state != LASER_DAMAGED )
      {
	 sprintf( buf, "Turret%d DAMAGED!\n\r", turretnum );
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(turret->roomvnum) , buf );
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->cockpit) , buf );
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->engineroom) , buf );
         turret->state = LASER_DAMAGED;
      }
      if ( number_range(1, 100) <= 5*ionFactor && ship->statettractor != LASER_DAMAGED && ship->mod->tractorbeam )
      {
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->pilotseat) , "Tractorbeam DAMAGED!" );
         ship->statettractor = LASER_DAMAGED;
      }
      if ( ions == FALSE )
      {
        ship->hull -= damage*5;
      }
    }

    if ( ship->hull <= 0 )
    {
        sprintf( buf, "%s(%s)", ship->name, ship->personalname );
        sprintf( logbuf , "%s was just destroyed by %s." , buf, ch->name );
	log_string( logbuf );


       xp =  ( exp_level( ch->skill_level[PILOTING_ABILITY]+1) - exp_level( ch->skill_level[PILOTING_ABILITY]) );
       xp = UMIN( get_ship_value( ship ) , xp );
       gain_exp( ch , xp , PILOTING_ABILITY);
       ch_printf( ch, "&WYou gain %ld piloting experience!\n\r", xp );

       if ( destroy_ship( ship , ch ) )
         return TRUE;

       return FALSE;
    }

    if ( ship->hull <= ship->mod->maxhull/20 )
       echo_to_cockpit( AT_BLOOD+ AT_BLINK , ship , "WARNING! Ship hull severely damaged!" );

    return FALSE;

}

bool damage_ship( SHIP_DATA *ship , SHIP_DATA *assaulter, int min , int max )
{
    int damage , shield_dmg;
    char buf[MAX_STRING_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
    sh_int ionFactor = 1;
    bool ions = FALSE;
    TURRET_DATA *turret;
    int turretnum;

    if ( min < 0 && max < 0 )
    {
      ions = TRUE;
      damage = number_range( max*(-1), min*(-1) );
    }
    else
      damage = number_range( min , max );

    if ( ions == TRUE )
      ionFactor = 2;

    if ( ship->shield > 0 )
    {
      shield_dmg = UMIN( ship->shield , damage );
 	   damage -= shield_dmg;
  	   ship->shield -= shield_dmg;
    	if ( ship->shield == 0 )
    	  echo_to_cockpit( AT_BLOOD , ship , "Shields down..." );
    }

    if ( ship->shield > 0 )
    {
        shield_dmg = UMIN( ship->shield , damage );
    	damage -= shield_dmg;
    	ship->shield -= shield_dmg;
    	if ( ship->shield == 0 )
    	  echo_to_cockpit( AT_BLOOD , ship , "Shields down..." );    	  
    }
    
    if ( damage > 0 )
    {
      if ( number_range(1, 100) <= 5*ionFactor && ship->shipstate != SHIP_DISABLED )
      {
         echo_to_cockpit( AT_BLOOD + AT_BLINK , ship , "Ships Drive DAMAGED!" );
         ship->shipstate = SHIP_DISABLED;
      }

      if ( number_range(1, 100) <= 5*ionFactor && ship->missilestate != MISSILE_DAMAGED && ship->mod->launchers> 0 )
      {
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Ships Missile Launcher DAMAGED!" );
         ship->missilestate = MISSILE_DAMAGED;
      }
      if ( number_range(1, 100) <= 2*ionFactor && ship->statet0 != LASER_DAMAGED )
      {
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->gunseat) , "Lasers DAMAGED!" );
         ship->statet0 = LASER_DAMAGED;
      }
      if ( ship->first_turret )
        for( turret = ship->first_turret, turretnum = 0; turret; turret = turret->next, turretnum++ )
          if ( number_range(1, 100) <= 5*ionFactor && turret->state != LASER_DAMAGED )
      {
	 sprintf( buf, "Turret%d DAMAGED!\n\r", turretnum );
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(turret->roomvnum) , buf );
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->cockpit) , buf );
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->engineroom) , buf );
         turret->state = LASER_DAMAGED;
      }
      if ( number_range(1, 100) <= 5*ionFactor && ship->statettractor != LASER_DAMAGED && ship->mod->tractorbeam )
      {
         echo_to_room( AT_BLOOD + AT_BLINK , get_room_index(ship->pilotseat) , "Tractorbeam DAMAGED!" );
         ship->statettractor = LASER_DAMAGED;
      }
      if ( ions == FALSE )
      {
        ship->hull -= damage*5;
      }
    }

    ship->hull -= damage*5;
    
    if ( ship->hull <= 0 )
    {
      sprintf( buf, "%s(%s)", ship->name, ship->personalname );
      sprintf( logbuf , "%s was just destroyed by %s." , ship->name, (assaulter ? assaulter->personalname : "a collision" ));
      log_string( logbuf );
      if ( destroy_ship( ship , NULL ) )
        return TRUE;
      return FALSE;
    }

    if ( ship->hull <= ship->mod->maxhull/20 )
       echo_to_cockpit( AT_BLOOD+ AT_BLINK , ship , "WARNING! Ship hull severely damaged!" );
       
    return FALSE;
}

bool destroy_ship( SHIP_DATA *ship , CHAR_DATA *ch )
{
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    char logbuf[MAX_STRING_LENGTH];
    int  roomnum;
    ROOM_INDEX_DATA *room;
    OBJ_DATA *robj;
    CHAR_DATA *rch;
    SHIP_DATA *lship;
    TURRET_DATA *turret;

    if (!ship)
      return TRUE;

    sprintf( buf , "%s explodes in a blinding flash of light!", ship->name );
    echo_to_system( AT_WHITE + AT_BLINK , ship , buf , NULL );

    if ( ship->shipclass == FIGHTER_SHIP )

    echo_to_ship( AT_WHITE + AT_BLINK , ship , "A blinding flash of light burns your eyes...");
    echo_to_ship( AT_WHITE , ship , "But before you have a chance to scream...\n\rYou are ripped apart as your spacecraft explodes...");

#ifdef NODEATH
    resetship(ship);
    makedebris(ship);
    return TRUE;
#endif
#ifdef NODEATHSHIP
    resetship(ship);
    makedebris(ship);
    return TRUE;
#endif

    if ( !str_cmp("Trainer", ship->owner))
    {
      resetship(ship);
      return TRUE;
    }

/*    explode_ship( ship ); */

    makedebris(ship);

    for ( roomnum = ship->firstroom ; roomnum <= ship->lastroom ; roomnum++ )
    {
        room = get_room_index(roomnum);

        if (room != NULL)
        {
         rch = room->first_person;
         while ( rch )
         {
            if ( IS_IMMORTAL(rch) )
            {
                 char_from_room(rch);
                 char_to_room( rch, get_room_index(wherehome(rch)) );
            }
            else
            {
              if ( ch )
                  raw_kill( ch , rch );
               else
                  raw_kill( rch , rch );
            }
            rch = room->first_person;
         }

         for ( robj = room->first_content ; robj ; robj = robj->next_content )
         {
           separate_obj( robj );
           extract_obj( robj );
         }
        }

    }

    for ( lship = first_ship; lship; lship = lship->next )
    {   
       if ( lship->docked && lship->docked == ship )
       {
	 if ( lship->location )
	   lship->shipstate = SHIP_LANDED;
	 else                
	  lship->shipstate = SHIP_READY;
          lship->docked = NULL;
          ship->docking = SHIP_READY;
       }    

       if ( !(ship->hanger) || (lship->location != ship->hanger) )
               continue;
       if ( ch )
       {
        sprintf( buf2, "%s(%s)", lship->name, lship->personalname );
        sprintf( logbuf , "%s was just destroyed by %s." , buf, ch->name );
	log_string( logbuf );
       }
       else
       {
        sprintf( buf2, "%s(%s)", lship->name, lship->personalname );
        sprintf( logbuf , "%s was just destroyed by a mob ship." , buf2);
        log_string( logbuf );
       }

	destroy_ship( lship, ch );
    }  

  for( lship = first_ship; lship; lship = lship->next )
  {
    if( lship->target0 == ship )
      lship->target0 = NULL;
    for( turret = lship->first_turret; turret; turret = turret->next )
      if( turret->target == ship )
        turret->target = NULL;
    if( lship->tractoredby == ship )
      lship->tractoredby = NULL;
    if( lship->tractored == ship )
      lship->tractored = NULL;
  }

  if ( ( ship->shipclass < CAPITAL_SHIP && ship->type != MOB_SHIP ) || ship->shipclass == SHIP_DEBRIS || is_rental(ch, ship) )
    shipdelete( ship, TRUE );
  else if ( ship->templatestring && ship->templatestring[0] != '\0' )
    shipdelete( ship, TRUE );
  else
    resetship ( ship );
  return TRUE;
  
}

bool ship_to_room(SHIP_DATA *ship , int vnum )
{
    ROOM_INDEX_DATA *shipto;
    
    if ( (shipto=get_room_index(vnum)) == NULL )
            return FALSE;
    LINK( ship, shipto->first_ship, shipto->last_ship, next_in_room, prev_in_room );
    ship->in_room = shipto; 
    return TRUE;
}


void do_board( CHAR_DATA *ch, char *argument )
{
   ROOM_INDEX_DATA *fromroom;
   ROOM_INDEX_DATA *toroom;
   SHIP_DATA *ship;
   SHIP_DATA *eShip = NULL;

   if ( ( ship = ship_from_entrance( ch->in_room->vnum ) ) != NULL )
   {
     eShip = get_ship( argument );
     if( !eShip || !eShip->docked || eShip->docked != ship )
       eShip = NULL;
       
     if ( !eShip && ship->docked == NULL )
       for( eShip = first_ship; eShip; eShip = eShip->next )
         if( eShip->docked && eShip->docked == ship && ( !argument || argument[0] == '\0'))
           break;

    if( !eShip && ship->docked && ( !argument || argument[0] == '\0') )
     eShip = ship->docked;

     if ( eShip && ship->docking == SHIP_READY && eShip->docking == SHIP_READY )
     {
       send_to_char( "You are not docked to a ship.\n\r", ch );
       return;
     }
     if ( eShip && ship->docking != SHIP_DOCKED && eShip->docking != SHIP_DOCKED )
     {
       send_to_char( "The docking manuever is still being completed.\n\r", ch );
       return;
     }

     if( eShip )
     {
      act( AT_PLAIN, "$n boards $T.", ch,
         NULL, eShip->name , TO_ROOM );
      act( AT_PLAIN, "You board $T.", ch,
         NULL, eShip->name , TO_CHAR );

         toroom = get_room_index( eShip->entrance );

	if ( !toroom )
	  toroom = eShip->cockpitroom;

         char_from_room( ch );
         char_to_room( ch, toroom );

           act( AT_PLAIN, "$n boards the ship.", ch,
             NULL, argument , TO_ROOM );

         do_look( ch , "auto" );
         return;
     }
   }


   if ( !argument || argument[0] == '\0')
   {
       send_to_char( "Board what?\n\r", ch );
       return;
   }


   if ( ( ship = ship_in_room( ch->in_room , argument ) ) == NULL )
   {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
   }

   if ( IS_SET( ch->act, ACT_MOUNTED ) && ( ch->position == POS_MOUNTED ) )
   {
          act( AT_PLAIN, "You can't go in there riding THAT.", ch, NULL, argument, TO_CHAR );
          return;
   } 

   fromroom = ch->in_room;
   toroom = get_room_index( ship->entrance );
   if( !toroom ) 
     toroom = ship->cockpitroom;

        if ( ( toroom ) != NULL )
   	{
   	   if ( ! ship->hatchopen )
   	   {
   	      send_to_char( "&RThe hatch is closed!\n\r", ch);
   	      return;
   	   }

           if ( toroom->tunnel > 0 )
           {
	        CHAR_DATA *ctmp;
	        int count = 0;

	       for ( ctmp = toroom->first_person; ctmp; ctmp = ctmp->next_in_room )
	       if ( ++count >= toroom->tunnel )
	       {
                  send_to_char( "There is no room for you in there.\n\r", ch );
		  return;
	       }
           }
            if ( ship->shipstate == SHIP_LAUNCH || ship->shipstate == SHIP_LAUNCH_2 )
            {
                 send_to_char("&rThat ship has already started launching!\n\r",ch);
                 return;
            }
            
            act( AT_PLAIN, "$n enters $T.", ch,
		NULL, ship->name , TO_ROOM );
	    act( AT_PLAIN, "You enter $T.", ch,
		NULL, ship->name , TO_CHAR );
   	    char_from_room( ch );
   	    char_to_room( ch , toroom );
   	    act( AT_PLAIN, "$n enters the ship.", ch,
		NULL, argument , TO_ROOM );
            do_look( ch , "auto" );

        }
        else
          send_to_char("That ship has no entrance!\n\r", ch);
}

bool rent_ship( CHAR_DATA *ch , SHIP_DATA *ship )
{   

    long price;

    if ( IS_NPC ( ch ) )
       return FALSE;

    price = get_ship_value( ship )/100;
    
    if( ship->cockpitroom )
     price = 0;
   
       if ( ch->gold < price )
       {
         ch_printf(ch, "&RRenting this ship costs %ld. You don't have enough credits!\n\r" , price );
         return FALSE;
       }

       ch->gold -= price;
       ch_printf(ch, "&GYou pay %ld credits to rent the ship.\n\r" , price );   
       return TRUE;
     
}

void do_leaveship( CHAR_DATA *ch, char *argument )
{
    ROOM_INDEX_DATA *fromroom;
    ROOM_INDEX_DATA *toroom;
    SHIP_DATA *ship;
    
    fromroom = ch->in_room;
    
    if  ( (ship = ship_from_entrance(fromroom->vnum)) == NULL )
    {
        send_to_char( "I see no exit here.\n\r" , ch );
        return;
    }   
    
    if  ( ship->shipclass == SHIP_PLATFORM )
    {
        send_to_char( "You can't do that here.\n\r" , ch );
        return;
    }   
    
    if ( ship->lastdoc != ship->location )
    {
        send_to_char("&rMaybe you should wait until the ship lands.\n\r",ch);
        return;
    }
    
    if ( ship->shipstate != SHIP_LANDED && ship->shipstate != SHIP_DISABLED )
    {
        send_to_char("&rPlease wait till the ship is properly docked.\n\r",ch);
        return;
    }
    
    if ( ! ship->hatchopen )
    {
    	send_to_char("&RYou need to open the hatch first" , ch );
    	return;
    }
    
    if ( ( toroom = get_room_index( ship->location ) ) != NULL )
    {
            act( AT_PLAIN, "$n exits the ship.", ch,
		NULL, argument , TO_ROOM );
	    act( AT_PLAIN, "You exit the ship.", ch,
		NULL, argument , TO_CHAR );
   	    char_from_room( ch );
   	    char_to_room( ch , toroom );
   	    act( AT_PLAIN, "$n steps out of a ship.", ch,
		NULL, argument , TO_ROOM );
            do_look( ch , "auto" );
            if ( ship->cockpitroom && !ship->cockpitroom->first_person && !str_cmp( ship->owner, "Public" ) )
            {
	      if ( IS_SET( ch->in_room->room_flags, ROOM_CAN_LAND ) )
	      {
      	        echo_to_room( AT_YELLOW , toroom , "The newly arrived rental is rolled into storage.\n\r" );
                dumpship( ship, toroom->vnum );
	        storeship(ship);
	      }
            }
     }       
     else
        send_to_char ( "The exit doesn't seem to be working properly.\n\r", ch );  
}

void do_launch( CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    SPACE_DATA *destin;

    int chance, shipclass; 
    long price = 0;
    SHIP_DATA *ship;
    char buf[MAX_STRING_LENGTH];
    ROOM_INDEX_DATA *room;

    argument = one_argument( argument , arg1);
    argument = one_argument( argument , arg2);

  if( arg1[0] != '\0' )
  {
    destin = spaceobject_from_name( arg1 );
    shipclass = atoi(arg2);
  }

            
    	        if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )  
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }

    	        if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }

    	        if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )
    	        {
    	            send_to_char("&RYou don't seem to be in the pilot seat!\n\r",ch);
    	            return;
    	        }

    	        if ( autofly(ship) )
    	        {
    	            send_to_char("&RThe ship is set on autopilot, you'll have to turn it off first.\n\r",ch);
    	            return;
    	        }

                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "You can't do that here.\n\r" , ch );
                   return;
                }

    	        if ( !check_pilot( ch , ship ) )
    	        {
    	            send_to_char("&RHey, thats not your ship! Try renting a public one.\n\r",ch);
    	            return;
    	        }

    	        if ( ship->lastdoc != ship->location )
                {
                     send_to_char("&rYou don't seem to be docked right now.\n\r",ch);
                     return;
                }

    	        if ( ship->tractoredby )
                {
                     send_to_char("&rYou are still locked in a tractor beam!\n\r",ch);
                     return;
                }

    	        if (ship->docking != SHIP_READY)
    	        {
    	            send_to_char("&RYou can't do that while docked to another ship!\n\r",ch);
    	            return;
    	        }
    	        if ( ship->shipstate != SHIP_LANDED && ship->shipstate != SHIP_DISABLED )
    	        {
    	            send_to_char("The ship is not docked right now.\n\r",ch);
    	            return;
    	        }

		if (ship->energy == 0)
		{
		  send_to_char("&RThis ship has no fuel, try installing a fuel module.\n\r",ch);
		  return;
		}	

    	        if ( ship->shipclass <= FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->shipclass == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
                if ( ship->shipclass == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( number_percent( ) < chance )
    		{  

                  if( 0 )
                  {
                    if(shipclass > 0 && shipclass < 4)
/*                      modtrainer( ship, shipclass )*/;
                    else
                    {
                       ch_printf(ch, "&RShip classes: fighter 1, midship 2, capital 3.\n\r");
                       return;
                    }
		    return;
                    if ( !(ship->destin) )
                    {
                       ch_printf(ch, "&RInvalid Trainer system.\n\r");
                       return;
                    }
                  }

    		   if ( is_rental(ch,ship) )
    		     if( !rent_ship(ch,ship) )
    		        return; 
    		    if ( !is_rental(ch,ship) )
                    {    
  		     if ( ship->shipclass == FIGHTER_SHIP )
                       price=2000;
                     if ( ship->shipclass == MIDSIZE_SHIP )
                       price=5000;
                     if ( ship->shipclass == CAPITAL_SHIP )
                       price=50000;
                      
                     price += ( ship->mod->maxhull-ship->hull );

                     if (ship->shipstate == SHIP_DISABLED )
                            price += 10000;
                     if ( ship->missilestate == MISSILE_DAMAGED )
                            price += 5000;
                     if ( ship->statet0 == LASER_DAMAGED )
                            price += 2500;
                    }                
                
		  if( IS_SET( ch->act, PLR_DONTAUTOFUEL ) )
		  {
		    if( ship->shipstate == SHIP_DISABLED )
                    {
                       ch_printf(ch, "Your ship is disabled. You must repair it.\n\r");
                       return;
                    }
		    
		    price = 100;
		  }
		    
    	          if ( ch->pcdata && ch->pcdata->clan && !str_cmp(ch->pcdata->clan->name,ship->owner) )
                  {
                   if ( ch->pcdata->clan->funds < price )
                   {
                       ch_printf(ch, "&R%s doesn't have enough funds to prepare this ship for launch.\n\r", ch->pcdata->clan->name );
                       return;
                   }

                   ch->pcdata->clan->funds -= price;
		   room = get_room_index( ship->location );
		   if( room != NULL && room->area )
		     boost_economy( room->area, price );
                   ch_printf(ch, "&GIt costs %s %ld credits to ready this ship for launch.\n\r", ch->pcdata->clan->name, price );
                  }
                  else if ( str_cmp( ship->owner , "Public" ) )
                  {
                   if ( ch->gold < price )
                   {
                       ch_printf(ch, "&RYou don't have enough funds to prepare this ship for launch.\n\r");
                       return;
                   }

                   ch->gold -= price;
		   room = get_room_index( ship->location );
		   if( room != NULL && room->area )
		     boost_economy( room->area, price );
                   ch_printf(ch, "&GYou pay %ld credits to ready the ship for launch.\n\r", price );

                  }

		 if( !IS_SET( ch->act, PLR_DONTAUTOFUEL ) )
		 {
		  if(  ship_from_hanger(ship->in_room->vnum) == NULL || ship->shipclass == SHIP_PLATFORM )
                    ship->energy = ship->mod->maxenergy;
       		  ship->shield = 0;
       		  ship->autorecharge = FALSE;
       		  ship->autotrack = FALSE;
       		  ship->autospeed = FALSE;
       		  ship->hull = ship->mod->maxhull;

       		  ship->missilestate = MISSILE_READY;
       		  ship->statet0 = LASER_READY;
       		  ship->shipstate = SHIP_LANDED;
		}
		
    		   if (ship->hatchopen)
    		   {
    		     ship->hatchopen = FALSE;
    		     sprintf( buf , "The hatch on %s closes." , ship->name);  
       	             echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
       	             echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch slides shut." );
       	             sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );
       	           }
    		   set_char_color( AT_GREEN, ch );
    		   send_to_char( "Launch sequence initiated.\n\r", ch);
    		   act( AT_PLAIN, "$n starts up the ship and begins the launch sequence.", ch,
		        NULL, argument , TO_ROOM );
		   echo_to_ship( AT_YELLOW , ship , "The ship hums as it lifts off the ground.");
    		   sprintf( buf, "%s begins to launch.", ship->name );
    		   echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
	           echo_to_docked( AT_YELLOW , ship, "The ship shudders as it lifts off the ground." );
    		   ship->shipstate = SHIP_LAUNCH;
    		   ship->goalspeed = ship->mod->realspeed;
    		   ship->accel = get_acceleration(ship);
    		   if ( ship->shipclass == FIGHTER_SHIP )
                      learn_from_success( ch, gsn_starfighters );
                   if ( ship->shipclass == MIDSIZE_SHIP )
                      learn_from_success( ch, gsn_midships );
                   if ( ship->shipclass == CAPITAL_SHIP )
                      learn_from_success( ch, gsn_capitalships );
                   sound_to_ship(ship , "!!SOUND(xwing)" );  
                   return;   	   	
                }
                set_char_color( AT_RED, ch );
	        send_to_char("You fail to work the controls properly!\n\r",ch);
	        if ( ship->shipclass == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
                if ( ship->shipclass == MIDSIZE_SHIP )
    	            learn_from_failure( ch, gsn_midships );
                if ( ship->shipclass == CAPITAL_SHIP )
                    learn_from_failure( ch, gsn_capitalships );
    	   	return;	

}

void launchship( SHIP_DATA *ship )
{   
    char buf[MAX_STRING_LENGTH];
    SHIP_DATA *target;
    int plusminus;
    
    ship_to_spaceobject( ship, spaceobject_from_vnum( ship->location ) );
    
    
    if ( ship->spaceobject == NULL )
    {
       echo_to_room( AT_YELLOW , get_room_index(ship->pilotseat) , "Launch path blocked .. Launch aborted.");
       echo_to_ship( AT_YELLOW , ship , "The ship slowly sets back back down on the landing pad.");
       sprintf( buf ,  "%s slowly sets back down." ,ship->name );
       echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf ); 
       ship->shipstate = SHIP_LANDED;
       return;
    }    
    
    if (ship->shipclass == MIDSIZE_SHIP)
    {
       sound_to_room( get_room_index(ship->location) , "!!SOUND(falcon)" );
       sound_to_ship(ship , "!!SOUND(falcon)" );
    }
    else if (ship->type == SHIP_IMPERIAL )
    {
       sound_to_ship(ship , "!!SOUND(tie)" );  
       sound_to_room( get_room_index(ship->location) , "!!SOUND(tie)" );
    }
    else
    {
       sound_to_ship(ship , "!!SOUND(xwing)" );  
       sound_to_room( get_room_index(ship->location) , "!!SOUND(xwing)" );
    }
    
    extract_ship(ship);    
    
    ship->location = 0;
    
    if (ship->shipstate != SHIP_DISABLED)
       ship->shipstate = SHIP_READY;
    
    plusminus = number_range ( -1 , 2 );
    if (plusminus > 0 )
        ship->hx = 1;
    else
        ship->hx = -1;
    
    plusminus = number_range ( -1 , 2 );
    if (plusminus > 0 )
        ship->hy = 1;
    else
        ship->hy = -1;
        
    plusminus = number_range ( -1 , 2 );
    if (plusminus > 0 )
        ship->hz = 1;
    else
        ship->hz = -1;
    
    if (ship->spaceobject && 
      ( ship->lastdoc == ship->spaceobject->doca  ||
        ship->lastdoc == ship->spaceobject->docb ||
        ship->lastdoc == ship->spaceobject->docc ) )
    {
       ship->vx = ship->spaceobject->xpos;
       ship->vy = ship->spaceobject->ypos;
       ship->vz = ship->spaceobject->zpos;
    }
    else
    {
       for ( target = first_ship; target; target = target->next )
       {
          if (ship->lastdoc == target->hanger)
          {
             ship->vx = target->vx;
             ship->vy = target->vy;
             ship->vz = target->vz;
          }
       }
    }
    
    ship->energy -= (100+100*ship->shipclass);
         
    ship->vx += (ship->hx*ship->currspeed*2);
    ship->vy += (ship->hy*ship->currspeed*2);
    ship->vz += (ship->hz*ship->currspeed*2);

    echo_to_room( AT_GREEN , get_room_index(ship->location) , "Launch complete.\n\r");	
    echo_to_ship( AT_YELLOW , ship , "The ship leaves the platform far behind as it flies into space." );
	// 2/18/04 - Johnson - Modified call to reflect origin of object entering the system
	// 2/19/04 - Johnson - TODO: Add code to reflect where in the system the ship launched from, e.g., a planet or hangar
    //sprintf( buf ,"%s enters the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
	SHIP_DATA* fromLocation = ship_from_hanger( ship->lastdoc );	// Get their last doc location (where they just launched from)
	if ( fromLocation != 0 )	// Make sure we dont use it if it's null for some reason
	{
		sprintf( buf ,"%s launches into the starsystem from %s at %.0f %.0f %.0f" , ship->name, fromLocation->name, ship->vx, ship->vy, ship->vz );
	}
	else
	{
		// fromLocation was null for somme reason, use the default
		sprintf( buf ,"%s launches into the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
	} // End changes for Johnson 2/19/04
    echo_to_system( AT_YELLOW, ship, buf , NULL ); 
    sprintf( buf, "%s lifts off into space.", ship->name );
    echo_to_room( AT_YELLOW , get_room_index(ship->lastdoc) , buf );
                 
}

void do_land( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *target;
    float vx, vy ,vz;
    char buf[MAX_STRING_LENGTH];
    SPACE_DATA *spaceobj;
    bool found = FALSE;
    
    strcpy( arg, argument );

    	        if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }

    	        if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }

    	        if ( (ship = ship_from_pilotseat(ch->in_room->vnum)) == NULL )
    	        {
    	            send_to_char("&RYou need to be in the pilot seat!\n\r",ch);
    	            return;
    	        }

    	        if ( autofly(ship) )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }

                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RYou can't land platforms\n\r" , ch );
                   return;
                }

    	        if (ship->shipclass == CAPITAL_SHIP)
    	        {
    	            send_to_char("&RCapital ships are to big to land. You'll have to take a shuttle.\n\r",ch);
		    return;
    	        }
    	        if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to land.\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate == SHIP_LANDED)
    	        {
                  if ( ship->docked == NULL )
                  {
                    send_to_char("&RThe ship is already docked!\n\r",ch);
    	              return;
                  }
    	        }
    	        if (ship->docking != SHIP_READY)
    	        {
    	            send_to_char("&RYou can't do that while docked to another ship!\n\r",ch);
    	            return;
    	        }

               if (ship->shipstate == SHIP_HYPERSPACE)
               {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;
               }

                if ( ship->tractoredby && ship->tractoredby->shipclass > ship->shipclass )
                {
                    send_to_char("&RYou can not move in a tractorbeam!\n\r",ch);
                    return;
                }
                if (ship->tractored && ship->tractored->shipclass > ship->shipclass )
                {
                    send_to_char("&RYou can not move while a tractorbeam is locked on to such a large mass.\n\r",ch);
                    return;
                }
    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current manouver.\n\r",ch);
    	            return;
    	        }

    	        if ( ship->energy < (25 + 25*ship->shipclass) )
    	        {
    	           send_to_char("&RTheres not enough fuel!\n\r",ch);
    	           return;
    	        }

    	        if ( argument[0] == '\0' )
    	        {
    	           set_char_color(  AT_CYAN, ch );
    	           ch_printf(ch, "%s" , "Land where?\n\r\n\rChoices: ");
	      for( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
	      {
	       if( space_in_range( ship, spaceobj ) )
	       {
    	        if ( spaceobj->doca && !spaceobj->seca)
    	             ch_printf(ch, "%s (%s)  %.0f %.0f %.0f\n\r         " ,
    	                        spaceobj->locationa,
    	                        spaceobj->name,
    	                        spaceobj->xpos,
    	                        spaceobj->ypos,
    	                        spaceobj->zpos );
    	        if ( spaceobj->docb && !spaceobj->secb )
    	             ch_printf(ch, "%s (%s)  %.0f %.0f %.0f\n\r         " ,
    	                        spaceobj->locationb,
    	                        spaceobj->name,
    	                        spaceobj->xpos,
    	                        spaceobj->ypos,
    	                        spaceobj->zpos );
    	        if ( spaceobj->docc && !spaceobj->secc )
    	             ch_printf(ch, "%s (%s)  %.0f %.0f %.0f\n\r         " ,
    	                        spaceobj->locationc,
    	                        spaceobj->name,
    	                        spaceobj->xpos,
    	                        spaceobj->ypos,
    	                        spaceobj->zpos );
    	       }
              }
                  ch_printf(ch, "\n\rYour Coordinates: %.0f %.0f %.0f\n\r" ,
                             ship->vx , ship->vy, ship->vz);
                   return;
    	        }

	      for( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
	       if( space_in_range( ship, spaceobj ) )
    	        if ( !str_prefix(argument,spaceobj->locationa) ||
    	             !str_prefix(argument,spaceobj->locationb) ||
    	             !str_prefix(argument,spaceobj->locationc))
    	        {
     	          found = TRUE;
     	          break;
     	        }

		if( !found )
		{
    	            target = get_ship_here( argument , ship );
    	            if ( target == NULL )
    	            {
    	                send_to_char("&RI don't see that here. Type land by itself for a list\n\r",ch);
    	                return;
    	            }
    	            if ( target == ship )
    	            {
    	                send_to_char("&RYou can't land your ship inside itself!\n\r",ch);
    	                return;
    	            }
    	            if ( ! target->hanger )
    	            {
    	                send_to_char("&RThat ship has no hanger for you to land in!\n\r",ch);
    	                return;
    	            }
    	            if ( ship->shipclass == MIDSIZE_SHIP && target->shipclass == MIDSIZE_SHIP )
    	            {
    	                send_to_char("&RThat ship is not big enough for your ship to land in!\n\r",ch);
    	                return;
    	            }
    	            if ( ! target->bayopen )
    	            {
    	                send_to_char("&RTheir hanger is closed. You'll have to ask them to open it for you\n\r",ch);
    	                return;
    	            }
    	            if (  (target->vx > ship->vx + 200) || (target->vx < ship->vx - 200) ||
    	                  (target->vy > ship->vy + 200) || (target->vy < ship->vy - 200) ||
    	                  (target->vz > ship->vz + 200) || (target->vz < ship->vz - 200) )
    	            {
    	                send_to_char("&R That ship is too far away! You'll have to fly a litlle closer.\n\r",ch);
    	                return;
    	            }
    	        }
                else
                {
		       ship->spaceobject = spaceobj;

                       vx = spaceobj->xpos;
                       vy = spaceobj->ypos;
                       vz = spaceobj->zpos;
    		    if (  (vx > ship->vx + 500) || (vx < ship->vx - 500) ||
    	                  (vy > ship->vy + 500) || (vy < ship->vy - 500) ||
    	                  (vz > ship->vz + 500) || (vz < ship->vz - 500) )
    	            {
    	                send_to_char("&R That platform is too far away! You'll have to fly a litlle closer.\n\r",ch);
    	                return;
    	            }                    
                }

                if ( ship->shipclass == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->shipclass == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
                if ( number_percent( ) < chance )
    		{
    		   set_char_color( AT_GREEN, ch );
    		   send_to_char( "Landing sequence initiated.\n\r", ch);
    		   act( AT_PLAIN, "$n begins the landing sequence.", ch,
		        NULL, argument , TO_ROOM );
            sprintf( buf ,"%s begins its landing sequence." , ship->name );
				echo_to_system( AT_YELLOW, ship, buf , NULL );
		   echo_to_docked( AT_YELLOW , ship, "The ship begins to enter the atmosphere." );

		   echo_to_ship( AT_YELLOW , ship , "The ship slowly begins its landing aproach.");
    		   ship->dest = STRALLOC(arg);
    		   ship->shipstate = SHIP_LAND;
    		   
    		   ship->goalspeed = 0;
    		   ship->accel = get_acceleration(ship);
    		       		   
	           if ( ship->shipclass == FIGHTER_SHIP )
                      learn_from_success( ch, gsn_starfighters );
                   if ( ship->shipclass == MIDSIZE_SHIP )
                      learn_from_success( ch, gsn_midships );
                   if ( spaceobject_from_vnum(ship->lastdoc) != ship->spaceobject )
                   {   
/*                      int xp =  (exp_level( ch->skill_level[PILOTING_ABILITY]+1) - exp_level( ch->skill_level[PILOTING_ABILITY])) ;
                      xp = UMIN( get_ship_value( ship ) , xp );
                      gain_exp( ch , xp , PILOTING_ABILITY );
                      ch_printf( ch, "&WYou gain %ld points of flight experience!\n\r", UMIN( get_ship_value( ship ) , xp ) );
*/
		      ship->ch = ch;                      
                   }
                   return;
	        }
	        send_to_char("You fail to work the controls properly.\n\r",ch);
	        if ( ship->shipclass == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
                else
    	            learn_from_failure( ch, gsn_midships );
    	   	return;	
}

void approachland( SHIP_DATA *ship, char *arg)
{
    SPACE_DATA *spaceobj;
    char buf[MAX_STRING_LENGTH];
    char buf2[MAX_STRING_LENGTH];
    bool found = FALSE;
    SHIP_DATA *target;
    
    for( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
      if( space_in_range( ship, spaceobj ) )
	if ( !str_prefix(arg,spaceobj->locationa) ||
    	     !str_prefix(arg,spaceobj->locationb) ||
    	     !str_prefix(arg,spaceobj->locationc))
    	{
     	    found = TRUE;
     	    break;
     	}

   if( found )
   {
    if ( !str_prefix(arg, spaceobj->locationa) )
       strcpy( buf2, spaceobj->locationa);
    else if ( !str_prefix(arg, spaceobj->locationb) )
       strcpy( buf2, spaceobj->locationb);
    else if ( !str_prefix(arg, spaceobj->locationc) )
       strcpy( buf2, spaceobj->locationc);
   }
    
    target = get_ship_here( arg , ship );
    if ( target != ship && target != NULL && target->bayopen 
            && ( ship->shipclass != MIDSIZE_SHIP || target->shipclass != MIDSIZE_SHIP ) )
       strcpy( buf2, target->name);

    if ( !found && !target )
    {
      echo_to_room( AT_YELLOW , get_room_index(ship->pilotseat), "ERROR");
      return;
    }

    sprintf( buf, "Approaching %s.", buf2 );    
    echo_to_room( AT_YELLOW , get_room_index(ship->pilotseat), buf);
    sprintf( buf, "%s begins its approach to %s.", ship->name, buf2 );    
    echo_to_system( AT_YELLOW, ship, buf , NULL );

    return;
}
    
void landship( SHIP_DATA *ship, char *arg )
{    
    SHIP_DATA *target;
    char buf[MAX_STRING_LENGTH];
    int destination;
    CHAR_DATA *ch;
    
    if ( !str_prefix(arg,ship->spaceobject->locationa) )
       destination = ship->spaceobject->doca;
    if ( !str_prefix(arg,ship->spaceobject->locationb) )
       destination = ship->spaceobject->docb;
    if ( !str_prefix(arg,ship->spaceobject->locationc) )
       destination = ship->spaceobject->docc;
    
    target = get_ship_here( arg , ship );
    if ( target != ship && target != NULL && target->bayopen 
            && ( ship->shipclass != MIDSIZE_SHIP || target->shipclass != MIDSIZE_SHIP ) )
    destination = target->hanger;
     
    if ( !ship_to_room( ship , destination ) )
    {
       echo_to_room( AT_YELLOW , get_room_index(ship->pilotseat), "Could not complete aproach. Landing aborted.");
       echo_to_ship( AT_YELLOW , ship , "The ship pulls back up out of its landing sequence.");
       if (ship->shipstate != SHIP_DISABLED)
           ship->shipstate = SHIP_READY;
       return;
    }      
         
    echo_to_room( AT_YELLOW , get_room_index(ship->pilotseat), "Landing sequence complete.");
    echo_to_ship( AT_YELLOW , ship , "You feel a slight thud as the ship sets down on the ground."); 
    sprintf( buf ,"%s disapears from your scanner." , ship->name  );
    echo_to_system( AT_YELLOW, ship, buf , NULL );

   if( ship->ch && ship->ch->desc )
   {
    int xp;
    
    ch = ship->ch;
    xp =  (exp_level( ch->skill_level[PILOTING_ABILITY]+1) - exp_level( ch->skill_level[PILOTING_ABILITY])) ;
    xp = UMIN( get_ship_value( ship ) , xp );
    gain_exp( ch , xp , PILOTING_ABILITY );
    ch_printf( ch, "&WYou gain %ld points of flight experience!\n\r", UMIN( get_ship_value( ship ) , xp ) );
    ship->ch = NULL;
   }


    ship->location = destination;
    ship->lastdoc = ship->location;
    if (ship->shipstate != SHIP_DISABLED)
       ship->shipstate = SHIP_LANDED;
    ship_from_spaceobject(ship, ship->spaceobject);
    if (ship->tractored)
    {
      if (ship->tractored->tractoredby == ship)
        ship->tractored->tractoredby = NULL;
      ship->tractored = NULL;
    }
    
    sprintf( buf, "%s lands on the platform.", ship->name );
    echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
    
    ship->energy = ship->energy - 25 - 25*ship->shipclass;
    
    if ( !str_cmp("Public",ship->owner) || !str_cmp("trainer",ship->owner) || ship->shipclass == SHIP_TRAINER )
    {
       ship->energy = ship->mod->maxenergy;
       ship->shield = 0;
       ship->autorecharge = FALSE;
       ship->autotrack = FALSE;
       ship->autospeed = FALSE;
       ship->hull = ship->mod->maxhull;
       
       ship->missilestate = MISSILE_READY;
       ship->statet0 = LASER_READY;
       ship->shipstate = SHIP_LANDED;
       
       echo_to_cockpit( AT_YELLOW , ship , "Repairing and refueling ship..." );
    }
    
       save_ship(ship);   
}

void do_accelerate( CHAR_DATA *ch, char *argument )
{
    int chance;
    int change;
    SHIP_DATA *ship;
    char buf[MAX_STRING_LENGTH];
    char arg1[MAX_STRING_LENGTH];
    int accel;
    
    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
                if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
                if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RThe controls must be at the pilots chair...\n\r",ch);
    	            return;
    	        }
                
                argument = one_argument( argument, arg1 );
                
                if( !argument || argument[0] == '\0' || !str_cmp(argument, "max") )
                  accel = get_acceleration( ship );
                else
                  accel = atoi( argument );
                  
                accel = URANGE( 0, accel, get_acceleration( ship ) );
                  
                ship->accel = accel;
                                
                if ( autofly(ship) )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RPlatforms can't move!\n\r" , ch );
                   return;
                }   

                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to accelerate.\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate == SHIP_LANDED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->docking != SHIP_READY)
    	        {
    	            send_to_char("&RYou can't do that while docked to another ship!\n\r",ch);
    	            return;
    	        }
                if ( ship->tractoredby && ship->tractoredby->shipclass > ship->shipclass )
                {
                    send_to_char("&RYou can not move in a tractorbeam!\n\r",ch);
                    return;
                }
                if (ship->tractored && ship->tractored->shipclass > ship->shipclass )
                {
                    send_to_char("&RYou can not move while a tractorbeam is locked on to such a large mass.\n\r",ch);
                    return;
                }
    	        if ( ship->energy < abs((atoi(arg1)-abs(ship->currspeed))/10) )
    	        {
    	           send_to_char("&RTheres not enough fuel!\n\r",ch);
    	           return;
    	        }
    	        
    	        if ( ship->shipclass == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->shipclass == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
/* changed mobs so they can not fly capital ships. Forcers could possess mobs
   and fly them - Darrik Vequir */
                if ( ship->shipclass == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? 0
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( number_percent( ) >= chance )
    		{
	           send_to_char("&RYou fail to work the controls properly.\n\r",ch);
	           if ( ship->shipclass == FIGHTER_SHIP )
                      learn_from_failure( ch, gsn_starfighters );
                   if ( ship->shipclass == MIDSIZE_SHIP )
    	              learn_from_failure( ch, gsn_midships );
                   if ( ship->shipclass == CAPITAL_SHIP )
                      learn_from_failure( ch, gsn_capitalships );
    	   	   return;	
                }
                
    change = atoi(arg1);
                      
    act( AT_PLAIN, "$n manipulates the ships controls.", ch,
    NULL, argument , TO_ROOM );

    ship->goalspeed = URANGE( 0 , change , ship->mod->realspeed );

    if ( change > ship->currspeed )
    {
       ship->inorbitof = NULL;
       send_to_char( "&GAccelerating\n\r", ch);
       echo_to_cockpit( AT_YELLOW , ship , "The ship begins to accelerate.");
       echo_to_docked( AT_YELLOW , ship, "The hull groans at an increase in speed." );
       sprintf( buf, "%s begins to speed up." , ship->name );
       echo_to_system( AT_ORANGE , ship , buf , NULL );
       ship->currspeed = URANGE( 0, (ship->currspeed+accel), ship->goalspeed );

    }
    
    if ( change < ship->currspeed )
    {
       send_to_char( "&GDecelerating\n\r", ch);
       echo_to_cockpit( AT_YELLOW , ship , "The ship begins to slow down.");
       echo_to_docked( AT_YELLOW , ship, "The hull groans as the ship slows." );
       sprintf( buf, "%s begins to slow down." , ship->name );
       echo_to_system( AT_ORANGE , ship , buf , NULL );
       ship->currspeed = URANGE( ship->goalspeed, (ship->currspeed-accel), ship->currspeed );
    }
    		     
    ship->energy -= abs((change-abs(ship->currspeed))/10);
    
    ship->goalspeed = URANGE( 0 , change , ship->mod->realspeed );
         
    if ( ship->shipclass == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->shipclass == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->shipclass == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    	
}

void do_trajectory_actual( CHAR_DATA *ch, char *argument )
{
    char  buf[MAX_STRING_LENGTH];
    char  arg2[MAX_INPUT_LENGTH];
    char  arg3[MAX_INPUT_LENGTH];
    int chance;
    float vx,vy,vz;
    SHIP_DATA *ship;
    
  
    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
                if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
                if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYour not in the pilots seat.\n\r",ch);
    	            return;
    	        }
                
                if ( autofly(ship))
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        }
                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RPlatforms can't turn!\n\r" , ch );
                   return;
                }   

    	        if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
    	        if (ship->shipstate == SHIP_DOCKED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current manouver.\n\r",ch);
    	            return;
    	        }
    	        if ( ship->energy < (ship->currspeed/10) )
    	        {
    	           send_to_char("&RTheres not enough fuel!\n\r",ch);
    	           return;
    	        }
    	        
                if ( ship->shipclass == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->shipclass == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;

/* changed mobs so they can not fly capital ships. Forcers could possess mobs
   and fly them - Darrik Vequir */
                if ( ship->shipclass == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? 0
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( number_percent( ) > chance )
    		{ 
	        send_to_char("&RYou fail to work the controls properly.\n\r",ch);
	        if ( ship->shipclass == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
                if ( ship->shipclass == MIDSIZE_SHIP )
    	            learn_from_failure( ch, gsn_midships );
                if ( ship->shipclass == CAPITAL_SHIP )
                    learn_from_failure( ch, gsn_capitalships );
    	   	return;	
    	        }
    	
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );
            
    vx = atof( arg2 );
    vy = atof( arg3 );
    vz = atof( argument );
            
    if ( vx == ship->vx && vy == ship->vy && vz == ship->vz )
    {
       ch_printf( ch , "The ship is already at %.0f %.0f %.0f !" ,vx,vy,vz);
    }

    if( vx == 0 && vy == 0 && vz == 0 )
    	vz = ship->vz+1;
                
    ship->hx = vx - ship->vx;
    ship->hy = vy - ship->vy;
    ship->hz = vz - ship->vz;
    
    ship->energy -= (ship->currspeed/10);
       
    ch_printf( ch ,"&GNew course set, aproaching %.0f %.0f %.0f.\n\r" , vx,vy,vz );            
    act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
                         
    echo_to_cockpit( AT_YELLOW ,ship, "The ship begins to turn.\n\r" );                        
    sprintf( buf, "%s turns altering its present course." , ship->name );
    echo_to_system( AT_ORANGE , ship , buf , NULL );
                                                            
    if ( ship->shipclass == FIGHTER_SHIP || ( ship->shipclass == MIDSIZE_SHIP && ship->mod->manuever > 50 ) )
        ship->shipstate = SHIP_BUSY_3;
    else if ( ship->shipclass == MIDSIZE_SHIP || ( ship->shipclass == CAPITAL_SHIP && ship->mod->manuever > 50 ) )
        ship->shipstate = SHIP_BUSY_2;
    else
        ship->shipstate = SHIP_BUSY;     
   
    if ( ship->shipclass == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->shipclass == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->shipclass == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    	
}

void do_trajectory( CHAR_DATA *ch, char *argument )
{
    char  buf[MAX_STRING_LENGTH];
    char  arg2[MAX_INPUT_LENGTH];
    char  arg3[MAX_INPUT_LENGTH];
    int chance;
    float vx,vy,vz;
    SHIP_DATA *ship;
    
  
    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
                if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
                if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYour not in the pilots seat.\n\r",ch);
    	            return;
    	        }
                
                if ( autofly(ship))
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        }
                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RPlatforms can't turn!\n\r" , ch );
                   return;
                }   

    	        if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
    	        if (ship->shipstate == SHIP_LANDED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->docking != SHIP_READY)
    	        {
    	            send_to_char("&RYou can't do that while docked to another ship!\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate != SHIP_READY && ship->shipstate != SHIP_TRACTORED)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current manouver.\n\r",ch);
    	            return;
    	        }
    	        if ( ship->energy < (ship->currspeed/10) )
    	        {
    	           send_to_char("&RTheres not enough fuel!\n\r",ch);
    	           return;
    	        }
    	        
                if ( ship->shipclass == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->shipclass == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;

/* changed mobs so they can not fly capital ships. Forcers could possess mobs
   and fly them - Darrik Vequir */
                if ( ship->shipclass == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? 0
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( number_percent( ) > chance )
    		{ 
	        send_to_char("&RYou fail to work the controls properly.\n\r",ch);
	        if ( ship->shipclass == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
                if ( ship->shipclass == MIDSIZE_SHIP )
    	            learn_from_failure( ch, gsn_midships );
                if ( ship->shipclass == CAPITAL_SHIP )
                    learn_from_failure( ch, gsn_capitalships );
    	   	return;	
    	        }
    	
    argument = one_argument( argument, arg2 );
    argument = one_argument( argument, arg3 );

    if( arg2 && arg2[0] != '\0' )
      vx = atof( arg2 );
    else
      vx = 0;

    if( arg3 && arg3[0] != '\0' )
      vy = atof( arg3 );
    else
      vy = 0;

    if( argument && argument[0] != '\0' )
      vz = atof( argument );
    else if ( vx != ship->vx && vy != ship->vy )
      vz = 0;
    else
      vz = 1;
            
    ship->hx = vx;
    ship->hy = vy;
    ship->hz = vz;
    
    ship->energy -= (ship->currspeed/10);
       
    ch_printf( ch ,"&GNew course set, aproaching %.0f %.0f %.0f.\n\r" , vx,vy,vz );            
    act( AT_PLAIN, "$n manipulates the ships controls.", ch, NULL, argument , TO_ROOM );
                         
    echo_to_cockpit( AT_YELLOW ,ship, "The ship begins to turn.\n\r" );                        
    sprintf( buf, "%s turns altering its present course." , ship->name );
    echo_to_system( AT_ORANGE , ship , buf , NULL );
                                                            
    if ( ship->shipclass == FIGHTER_SHIP || ( ship->shipclass == MIDSIZE_SHIP && ship->mod->manuever > 50 ) )
        ship->shipstate = SHIP_BUSY_3;
    else if ( ship->shipclass == MIDSIZE_SHIP || ( ship->shipclass == CAPITAL_SHIP && ship->mod->manuever > 50 ) )
        ship->shipstate = SHIP_BUSY_2;
    else
        ship->shipstate = SHIP_BUSY;     
   
    if ( ship->shipclass == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->shipclass == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->shipclass == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    	
}

void do_buyship(CHAR_DATA *ch, char *argument )
{
    long         price;
    SHIP_DATA   *ship;

   if ( IS_NPC(ch) || !ch->pcdata )
   {
   	send_to_char( "&ROnly players can do that!\n\r" ,ch );
   	return;
   }

   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
      ship = ship_from_cockpit( ch->in_room->vnum );
      
      if ( !ship )
      {            
           act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
      }
   }
   
   if ( str_cmp( ship->owner , "" )  || ship->type == MOB_SHIP )
   {
   	send_to_char( "&RThat ship isn't for sale!" ,ch );
   	return;
   }


   if ( ship->type == SHIP_IMPERIAL )
   {
        if ( !ch->pcdata->clan || str_cmp( ch->pcdata->clan->name , "the empire" ) )
        {
   	 if ( !ch->pcdata->clan || !ch->pcdata->clan->mainclan || str_cmp( ch->pcdata->clan->mainclan->name , "The Empire" ) )
   	 {
           send_to_char( "&RThat ship may only be purchaced by the Empire!\n\r" ,ch );
   	   return;
   	 }
   	}
   }
   else if ( ship->type == SHIP_REBEL )
   {
        if ( !ch->pcdata->clan || (str_cmp( ch->pcdata->clan->name , "the rebel alliance" ) && str_cmp( ch->pcdata->clan->name , "The New Republic")))
        {
         if ( !ch->pcdata->clan ||  !ch->pcdata->clan->mainclan || str_cmp( ch->pcdata->clan->mainclan->name , "The Rebel Alliance" ) ) 
   	 {
   	  send_to_char( "&RThat ship may only be purchaced by The Rebel Alliance!\n\r" ,ch );
   	  return;
         }
   	}
   }
      
   price = get_ship_value( ship );
    
    if ( ch->gold < price )
    {
       ch_printf(ch, "&RThis ship costs %ld. You don't have enough credits!\n\r" , price );
       return;
    }
    
    ch->gold -= price;
    ch_printf(ch, "&GYou pay %ld credits to purchace the ship.\n\r" , price );   

    act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch,
       NULL, argument , TO_ROOM );
    
    	STRFREE( ship->owner );
	ship->owner = STRALLOC( ch->name );
	save_ship( ship );
                 
}

void do_clanbuyship(CHAR_DATA *ch, char *argument )
{
    long         price;
    SHIP_DATA   *ship;
    CLAN_DATA   *clan;
    CLAN_DATA   *mainclan;
       
   if ( IS_NPC(ch) || !ch->pcdata )
   {
   	send_to_char( "&ROnly players can do that!\n\r" ,ch );
   	return;
   }
   if ( !ch->pcdata->clan )
   {
   	send_to_char( "&RYou aren't a member of any organizations!\n\r" ,ch );
   	return;
   }
   
   clan = ch->pcdata->clan;
   mainclan = ch->pcdata->clan->mainclan ? ch->pcdata->clan->mainclan : clan;
   
   if ( ( ch->pcdata->bestowments
    &&    is_name("clanbuyship", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->leader  ))
	;
   else
   {
   	send_to_char( "&RYour organization hasn't seen fit to bestow you with that ability.\n\r" ,ch );
   	return;
   }
	
   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
      ship = ship_from_cockpit( ch->in_room->vnum );
      
      if ( !ship )
      {            
           act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
      }
   }

   if ( str_cmp( ship->owner , "" )  || ship->type == MOB_SHIP )
   {
   	send_to_char( "&RThat ship isn't for sale!\n\r" ,ch );
   	return;
   }
   
   if ( str_cmp( mainclan->name , "The Empire" )  && ship->type == SHIP_IMPERIAL ) 
   {
   	   send_to_char( "&RThat ship may only be purchaced by the Empire!\n\r" ,ch );
   	   return;
   }
   
   if ( (str_cmp( mainclan->name , "The Rebel Alliance" ) && str_cmp( mainclan->name , "The New Republic" ) )  && ship->type == SHIP_REBEL )
   {
   	send_to_char( "&RThat ship may only be purchaced by The Rebel Alliance!\n\r" ,ch );
   	return;
   }
   
   if ( !str_cmp( clan->name , "The Empire" )  && ship->type != SHIP_IMPERIAL )
   {
   	send_to_char( "&RDue to contractual agreements that ship may not be purchaced by the empire!\n\r" ,ch );
   	return;
   }
   
   price = get_ship_value( ship );
    
    if ( ch->pcdata->clan->funds < price )
    {
       ch_printf(ch, "&RThis ship costs %ld. You don't have enough credits!\n\r" , price );
       return;
    }
    
    clan->funds -= price;
    ch_printf(ch, "&G%s pays %ld credits to purchace the ship.\n\r", clan->name , price );   

    act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch,
       NULL, argument , TO_ROOM );
    
    	STRFREE( ship->owner );
	ship->owner = STRALLOC( clan->name );
	save_ship( ship );
               
   if ( ship->shipclass <= SHIP_PLATFORM )
             clan->spacecraft++;
   else
             clan->vehicles++;
}

void do_clansellship(CHAR_DATA *ch, char *argument )
{
    long         price;
    SHIP_DATA   *ship;
    CLAN_DATA   *clan;
    CLAN_DATA   *mainclan;
       
   if ( IS_NPC(ch) || !ch->pcdata )
   {
   	send_to_char( "&ROnly players can do that!\n\r" ,ch );
   	return;
   }
   if ( !ch->pcdata->clan )
   {
   	send_to_char( "&RYou aren't a member of any organizations!\n\r" ,ch );
   	return;
   }
   
   clan = ch->pcdata->clan;
   mainclan = ch->pcdata->clan->mainclan ? ch->pcdata->clan->mainclan : clan;
   
   if ( ( ch->pcdata->bestowments
    &&    is_name("clanbuyship", ch->pcdata->bestowments))
    ||   !str_cmp( ch->name, clan->leader  ))
	;
   else
   {
   	send_to_char( "&RYour organization hasn't seen fit to bestow you with that ability.\n\r" ,ch );
   	return;
   }
	
   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
      ship = ship_from_cockpit( ch->in_room->vnum );
      
      if ( !ship )
      {            
           act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
      }
   }

   if ( !str_cmp( ship->owner , "" )  || ship->type == MOB_SHIP )
   {
   	send_to_char( "&RThat ship is not owned!\n\r" ,ch );
   	return;
   }

   if ( str_cmp( ship->owner , ch->pcdata->clan->name ) )
   {
   	send_to_char( "&RThat isn't your ship!" ,ch );
   	return;
   }

   price = get_ship_value( ship );
    
    ch->pcdata->clan->funds += ( price - price/10 );
    ch_printf(ch, "&GYour clan receives %ld credits from selling your ship.\n\r" , price - price/10 );   

    act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch,
       NULL, argument , TO_ROOM );
 
	STRFREE( ship->owner );
	ship->owner = STRALLOC( "" );
	ship->pilot = STRALLOC( "" );
	ship->copilot = STRALLOC( "" );
	save_ship( ship );

}


void do_sellship(CHAR_DATA *ch, char *argument )
{
    long         price;
    SHIP_DATA   *ship;

   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
   }

   if ( str_cmp( ship->owner , ch->name ) )
   {
   	send_to_char( "&RThat isn't your ship!" ,ch );
   	return;
   }

   price = get_ship_value( ship );
    
    ch->gold += ( price - price/10 );
    ch_printf(ch, "&GYou receive %ld credits from selling your ship.\n\r" , price - price/10 );   

    act( AT_PLAIN, "$n walks over to a terminal and makes a credit transaction.",ch,
       NULL, argument , TO_ROOM );
 
	STRFREE( ship->owner );
	ship->owner = STRALLOC( "" );
	ship->pilot = STRALLOC( "" );
	ship->copilot = STRALLOC( "" );
	save_ship( ship );

}

void do_info(CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    SHIP_DATA *target;
    bool fromafar = TRUE;
    
    if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    {
            if ( argument[0] == '\0' )
            {
               act( AT_PLAIN, "Which ship do you want info on?.", ch, NULL, NULL, TO_CHAR );
               return;
            }

            ship = ship_in_room( ch->in_room , argument );
            if ( !ship )
            {
               act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
               return;
            }
            target = ship;
    }
    else if ( ship->hanger == ch->in_room->vnum )
    {
            if ( argument[0] == '\0' )
            {
               act( AT_PLAIN, "Which ship do you want info on?.", ch, NULL, NULL, TO_CHAR );
               return;
            }

            ship = ship_in_room( ch->in_room , argument );
            if ( !ship )
            {
               act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
               return;
            }

            target = ship;
    }

    else if (argument[0] == '\0')
    {
       target = ship;
       fromafar = FALSE;
    }
    else
       target = get_ship_here( argument , ship );

    if ( target == NULL )
    {
         send_to_char("&RI don't see that here.\n\rTry the radar, or type info by itself for info on this ship.\n\r",ch);
         return;
    }

    if ( check_pilot( ch , target ) )
      fromafar = FALSE;

    if ( abs( (int) ( target->vx - ship->vx )) > 500+ship->mod->sensor*2 ||
         abs( (int) ( target->vy - ship->vy )) > 500+ship->mod->sensor*2 ||
         abs( (int) ( target->vz - ship->vz )) > 500+ship->mod->sensor*2 )
    {
         send_to_char("&RThat ship is to far away to scan.\n\r",ch);
         return;
    }

    ch_printf( ch, "&Y%s %s : %s (%s)\n\r&B",
			target->type == SHIP_REBEL ? "Rebel" :
		       (target->type == SHIP_IMPERIAL ? "Imperial" : "Civilian" ),
		        target->shipclass == FIGHTER_SHIP ? "Starfighter" :
		       (target->shipclass == MIDSIZE_SHIP ? "Midtarget" : 
		       (target->shipclass == CAPITAL_SHIP ? "Capital Ship" :
		       (ship->shipclass == SHIP_PLATFORM ? "Platform" : 
		       (ship->shipclass == CLOUD_CAR ? "Cloudcar" : 
		       (ship->shipclass == OCEAN_SHIP ? "Boat" : 
		       (ship->shipclass == LAND_SPEEDER ? "Speeder" : 
		       (ship->shipclass == WHEELED ? "Wheeled Transport" : 
		       (ship->shipclass == LAND_CRAWLER ? "Crawler" : 
		       (ship->shipclass == WALKER ? "Walker" : "Unknown" ) ) ) ) ) ) ) ) ),
    			target->name,
    			target->personalname,
    			target->filename);
    ch_printf( ch, "Description: %s\n\rOwner: %s",
    			target->description,
    			target->owner );
   if( fromafar == FALSE )
    ch_printf( ch, "   Pilot: %s   Copilot: %s", target->pilot,  target->copilot );
    ch_printf( ch, "\n\rLaser cannons: %d  Ion cannons: %d\n\r",
    			target->mod->lasers, target->mod->ions);
    ch_printf( ch, "Projectile Launchers (8): %d  ",
       			target->mod->launchers);
    ch_printf( ch, "Chaff Launchers (6): %d\n\r",
       			target->mod->defenselaunchers);
    ch_printf( ch, "Max Hull: %d  ",
                        target->mod->maxhull);
    ch_printf( ch, "Max Shields: %d   Max Energy(fuel): %d\n\r",
                        target->mod->maxshield,
    		        target->mod->maxenergy);
    ch_printf( ch, "Maximum Speed: %d   Hyperspeed: %d  Value: %d\n\r",
                        target->mod->realspeed, target->mod->hyperspeed, get_ship_value( target ));
    ch_printf( ch, "Maximum Cargo: %d\n\r", target->maxcargo );

    act( AT_PLAIN, "$n checks various gages and displays on the control panel.", ch,
         NULL, argument , TO_ROOM );
	  
}

void do_autorecharge(CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    int recharge;


        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
        if (  (ship = ship_from_coseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the co-pilots seat!\n\r",ch);
            return;
        }
        
        if ( autofly(ship)  )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	        
        chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
        if ( number_percent( ) > chance )
        {
           send_to_char("&RYou fail to work the controls properly.\n\r",ch);
           learn_from_failure( ch, gsn_shipsystems );
           return;	
        }
    
    act( AT_PLAIN, "$n flips a switch on the control panell.", ch,
         NULL, argument , TO_ROOM );

    if ( !str_cmp(argument,"on" ) )
    {
        ship->autorecharge=TRUE;
        send_to_char( "&GYou power up the shields.\n\r", ch);
        echo_to_cockpit( AT_YELLOW , ship , "Shields ON. Autorecharge ON.");
    }
    else if ( !str_cmp(argument,"off" ) )
    {
        ship->autorecharge=FALSE;
        send_to_char( "&GYou shutdown the shields.\n\r", ch);
        echo_to_cockpit( AT_YELLOW , ship , "Shields OFF. Shield strength set to 0. Autorecharge OFF.");
        ship->shield = 0;
    }
    else if ( !str_cmp(argument,"idle" ) )
    {
        ship->autorecharge=FALSE;
        send_to_char( "&GYou let the shields idle.\n\r", ch);
        echo_to_cockpit( AT_YELLOW , ship , "Autorecharge OFF. Shields IDLEING.");
    }
    else
    {   
        if (ship->autorecharge == TRUE)
        {
           ship->autorecharge=FALSE;
           send_to_char( "&GYou toggle the shields.\n\r", ch);
           echo_to_cockpit( AT_YELLOW , ship , "Autorecharge OFF. Shields IDLEING.");
        }
        else
        {
           ship->autorecharge=TRUE;
           send_to_char( "&GYou toggle the shields.\n\r", ch);
           echo_to_cockpit( AT_YELLOW , ship , "Shields ON. Autorecharge ON");
        }   
    }
    
    if (ship->autorecharge)
    {
       recharge  = URANGE( 1, ship->mod->maxshield-ship->shield, 25+ship->shipclass*25 );
       recharge  = UMIN( recharge, ship->energy*5 + 100 );
       ship->shield += recharge;
       ship->energy -= ( recharge*2 + recharge * ship->shipclass );
    }
                                          	  
    learn_from_success( ch, gsn_shipsystems );    	
}

void do_autopilot(CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
        
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
        if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the pilots seat!\n\r",ch);
            return;
        }
        
         if ( !check_pilot(ch,ship) )
       	     {
       	       send_to_char("&RHey! Thats not your ship!\n\r",ch);
       	       return;
       	     }

         if ( ship->shipstate == SHIP_DOCKED )
         {
           if(ship->docked == NULL || ( ship->docked->shipclass > MIDSIZE_SHIP && ship->shipclass > MIDSIZE_SHIP ))
           {
             send_to_char("&RNot until after you've launched!\n\r",ch);
             return;
           }
           send_to_char("&RNot while you are docked!\n\r",ch);
           return;
         }

         if ( ship->target0 )
       	     {
		ship->autotrack = FALSE;
      	     }
  
        
    act( AT_PLAIN, "$n flips a switch on the control panell.", ch,
         NULL, argument , TO_ROOM );

        if ( ( ship->autopilot == TRUE && str_cmp(argument,"on") )
        || !str_cmp(argument,"off") )
        {
           ship->autopilot=FALSE;
           send_to_char( "&GYou toggle the autopilot.\n\r", ch);
           echo_to_cockpit( AT_YELLOW , ship , "Autopilot OFF.");
        }
        else
        {
           if( ship->shipstate == SHIP_LANDED )
         {
           send_to_char("&RNot while you are docked!\n\r",ch);
           return;
         }
           ship->autopilot=TRUE;
           ship->autorecharge = TRUE;
           send_to_char( "&GYou toggle the autopilot.\n\r", ch);
           echo_to_cockpit( AT_YELLOW , ship , "Autopilot ON.");
        }   

}

void do_openhatch(CHAR_DATA *ch, char *argument )
{
   SHIP_DATA *ship;
   char buf[MAX_STRING_LENGTH];

   if ( !argument || argument[0] == '\0' || !str_cmp(argument,"hatch") )
   {
       ship = ship_from_entrance( ch->in_room->vnum );
       if( ship == NULL)
       {
          send_to_char( "&ROpen what?\n\r", ch );
          return;
       }
       else
       {
          if ( !ship->hatchopen)
       	  {

                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RTry one of the docking bays!\n\r" , ch );
                   return;
                }
       	     if ( ship->location != ship->lastdoc ||
       	        ( ship->shipstate != SHIP_LANDED && ship->shipstate != SHIP_DISABLED ) )
       	     {
       	       send_to_char("&RPlease wait till the ship lands!\n\r",ch);
       	       return;
       	     }
       	     ship->hatchopen = TRUE;
       	     send_to_char("&GYou open the hatch.\n\r",ch);
       	     act( AT_PLAIN, "$n opens the hatch.", ch, NULL, argument, TO_ROOM );
       	     sprintf( buf , "The hatch on %s opens." , ship->name);
       	     echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
       	       sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );
		return;
       	  }
       	  else
       	  {
       	     send_to_char("&RIt's already open.\n\r",ch);
       	     return;
       	  }
       }
   }

   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
   }

   if ( ship->shipstate != SHIP_LANDED && ship->shipstate != SHIP_DISABLED )
   {
        send_to_char( "&RThat ship has already started to launch",ch);
        return;
   }
   
   if ( ! check_pilot(ch,ship) )
       	     {
       	       send_to_char("&RHey! Thats not your ship!\n\r",ch);
       	       return;
       	     }
       	     
   if ( !ship->hatchopen)
   {
   	ship->hatchopen = TRUE;
   	act( AT_PLAIN, "You open the hatch on $T.", ch, NULL, ship->name, TO_CHAR );
   	act( AT_PLAIN, "$n opens the hatch on $T.", ch, NULL, ship->name, TO_ROOM );
   	echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch opens from the outside." );
   	  sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );		
	return;
   }

   send_to_char("&GIts already open!\n\r",ch);

}


void do_closehatch(CHAR_DATA *ch, char *argument )
{
   SHIP_DATA *ship;
   char buf[MAX_STRING_LENGTH];
   
   if ( !argument || argument[0] == '\0' || !str_cmp(argument,"hatch") )
   {
       ship = ship_from_entrance( ch->in_room->vnum );
       if( ship == NULL)
       {
          send_to_char( "&RClose what?\n\r", ch );
          return;
       }
       else
       {
          
                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RTry one of the docking bays!\n\r" , ch );
                   return;
                }   
          if ( ship->hatchopen)
       	  {
       	     ship->hatchopen = FALSE;
       	     send_to_char("&GYou close the hatch.\n\r",ch);
       	     act( AT_PLAIN, "$n closes the hatch.", ch, NULL, argument, TO_ROOM );  
       	     sprintf( buf , "The hatch on %s closes." , ship->name);  
       	     echo_to_room( AT_YELLOW , get_room_index(ship->location) , buf );
       	      sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );		
 	     return;
       	  }
       	  else
       	  {
       	     send_to_char("&RIt's already closed.\n\r",ch);
       	     return;
       	  }
       }
   }
   
   ship = ship_in_room( ch->in_room , argument );
   if ( !ship )
   {
            act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
           return;
   }

   if ( ship->shipstate != SHIP_LANDED && ship->shipstate != SHIP_DISABLED )
   {
        send_to_char( "&RThat ship has already started to launch",ch);
        return;
   }
   else
   {
      if(ship->hatchopen)
      {
   	ship->hatchopen = FALSE;
   	act( AT_PLAIN, "You close the hatch on $T.", ch, NULL, ship->name, TO_CHAR );
   	act( AT_PLAIN, "$n closes the hatch on $T.", ch, NULL, ship->name, TO_ROOM );
        echo_to_room( AT_YELLOW , get_room_index(ship->entrance) , "The hatch is closed from outside.");
     sound_to_room( get_room_index(ship->entrance) , "!!SOUND(door)" );
      		     sound_to_room( get_room_index(ship->location) , "!!SOUND(door)" );		

   	return;
      }
      else
      {
      	send_to_char("&RIts already closed.\n\r",ch);
      	return;
      }
   }


}

void do_status(CHAR_DATA *ch, char *argument )
{
    int chance, turretnum;
    SHIP_DATA *ship;
    SHIP_DATA *target;
    TURRET_DATA *turret;
    
   
    if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    {
         send_to_char("&RYou must be in the cockpit, turret or engineroom of a ship to do that!\n\r",ch);
         return;
    }
    
    if (argument[0] == '\0')
       target = ship;
    else
       target = get_ship_here( argument , ship );
    	
    if ( target == NULL )
    {
         send_to_char("&RI don't see that here.\n\rTry the radar, or type status by itself for your ships status.\n\r",ch);
         return;
    }          
    
    if ( abs( (int) ( target->vx - ship->vx )) > 500+ship->mod->sensor*2 ||
         abs( (int) ( target->vy - ship->vy )) > 500+ship->mod->sensor*2 ||
         abs( (int) ( target->vz - ship->vz )) > 500+ship->mod->sensor*2 )  
    {
         send_to_char("&RThat ship is to far away to scan.\n\r",ch);
         return;
    }
    
    chance = IS_NPC(ch) ? ch->top_level
        : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
    if ( number_percent( ) > chance )
    {
        send_to_char("&RYou cant figure out what the readout means.\n\r",ch);
        learn_from_failure( ch, gsn_shipsystems );
        return;	
    }
        
    act( AT_PLAIN, "$n checks various gages and displays on the control panel.", ch,
         NULL, argument , TO_ROOM );
    
    ch_printf( ch, "&W%s:\n\r",target->name);
    ch_printf( ch, "&OCurrent Coordinates:&Y %.0f %.0f %.0f\n\r",
                        target->vx, target->vy, target->vz );
    ch_printf( ch, "&OCurrent Heading:&Y %.0f %.0f %.0f\n\r",
                        target->hx, target->hy, target->hz );
    ch_printf( ch, "&OCurrent Speed:&Y %d&O/%d Accel: &Y%d&O/%d\n\r",
                        target->currspeed , target->mod->realspeed, 
                        target->accel, get_acceleration(target) );
    ch_printf( ch, "&OHull:&Y %d&O/%d  Ship Condition:&Y %s\n\r",
                        target->hull,
    		        target->mod->maxhull,
    			target->shipstate == SHIP_DISABLED ? "Disabled" : "Running");
    ch_printf( ch, "&OShields:&Y %d&O/%d   Energy(fuel):&Y %d&O/%d\n\r",
                        target->shield,
    		        target->mod->maxshield,
    		        target->energy,
    		        target->mod->maxenergy);
    ch_printf( ch, "&OLaser Condition:&Y %s  &OCurrent Target:&Y %s\n\r",
    		        target->statet0 == LASER_DAMAGED ? "Damaged" : "Good" , target->target0 ? target->target0->name : "none");
    if (target->first_turret)
      for ( turret = target->first_turret, turretnum = 0; turret; turret= turret->next, turretnum++ )
        ch_printf( ch, "&OTurret %d:  &Y %s  &OCurrent Target:&Y %s\n\r",
    			turretnum, turret->state == LASER_DAMAGED ? "Damaged" : "Good" , turret->target ? turret->target->name : "none");

    ch_printf( ch, "&OSensors:    &Y%d   &OTractor Beam:   &Y%d\n\r", target->mod->sensor, target->mod->tractorbeam);
    ch_printf( ch, "&OAstroArray: &Y%d   &OComm:           &Y%d\n\r", target->mod->astro_array, target->mod->comm);
   if( target->mod->gravitypower > 0 )
    ch_printf( ch, "&OGravity Well Projectors: Power: &Y%d\n\r", target->mod->gravitypower );
    ch_printf( ch, "\n\r&OMissiles:&Y %d&O  Torpedos: &Y%d&ORockets:  &Y%d&O\n\r  Chaff:    &Y%d&O  Gravity Power: %d/%d\n\r Launchers: %d/%d Condition:&Y %s&w\n\r",
       			target->missiles,
    			target->torpedos,
    			target->rockets,
            target->chaff, ship->mod->gravproj, ship->mod->gravitypower,
            target->mod->launchers,
            target->mod->defenselaunchers,
    			target->missilestate == MISSILE_DAMAGED ? "Damaged" : "Good");
    ch_printf( ch, "\n\r&OMaxMods Ext:&Y %d&O/%d  Int: &Y%d&O/%d\n\r", 
               get_extmodule_count(target), target->maxextmodules, get_intmodule_count(target), target->maxintmodules );

    learn_from_success( ch, gsn_shipsystems );
}

void do_hyperspace(CHAR_DATA *ch, char *argument )
{
    int chance;
    float tx, ty, tz;
    SHIP_DATA *ship;
    SHIP_DATA *dship, *target;
    SPACE_DATA *spaceobject;
    char buf[MAX_STRING_LENGTH];
   
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
        if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }


        if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou aren't in the pilots seat.\n\r",ch);
            return;
        }

        if ( autofly(ship)  )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }

                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RPlatforms can't move!\n\r" , ch );
                   return;
                }
                if (ship->mod->hyperspeed == 0)
                {
                  send_to_char("&RThis ship is not equipped with a hyperdrive!\n\r",ch);
                  return;
                }
                if (( !argument || argument[0] == '\0' || str_cmp( argument, "off" )) && ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou are already travelling lightspeed!\n\r",ch);
                  return;
                }
		if ( ship->mod->gravproj )
		{
		  send_to_char ("&RYou can not enter hyperspace with gravity wells activated!\n\r", ch);
		  return;
		}

		for( target = first_ship; target; target= target->next )
		  if (target && ship && target != ship ) 
		  {
		    if ( target->spaceobject && ship->spaceobject &&  target->shipstate != SHIP_LANDED && 
		      target->mod && target->mod->gravitypower && target->mod->gravproj &&
		        abs( (int) ( ship->vx - target->vx )) < 10*target->mod->gravproj &&
		        abs( (int) ( ship->vy - target->vy )) < 10*target->mod->gravproj &&
		        abs( (int) ( ship->vz - target->vz )) < 10*target->mod->gravproj )
		    {
		      ch_printf(ch, "You are still within the %s's artificial gravity well.\n\r", target->name );
		      return;
		    }
		  }
                
                if ( argument && !str_cmp( argument, "off" ) &&  ship->shipstate != SHIP_HYPERSPACE )
                {
                  send_to_char("&RHyperdrive not active.\n\r",ch);
                  return;
                }
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate == SHIP_LANDED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->docking != SHIP_READY )
    	        {
    	            send_to_char("&RYou can't do that while docked to another ship!\n\r",ch);
    	            return;
    	        }
                if (ship->tractoredby || ship->tractored )
                {
                    send_to_char("&RYou can not move in a tractorbeam!\n\r",ch);
                    return;
                }
                if (ship->tractored && ship->tractored->shipclass > ship->shipclass )
                {
                    send_to_char("&RYou can not enter hyperspace with your tractor beam locked on.\n\r",ch);
                    return;
                }
    	        if (ship->shipstate != SHIP_READY && ship->shipstate != SHIP_HYPERSPACE)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current manouver.\n\r",ch);
    	            return;
    	        }
               if ( argument && !str_cmp( argument, "off" ) &&  ship->shipstate == SHIP_HYPERSPACE)
               {
            	ship_to_spaceobject (ship, ship->currjump);
            	
            	if (ship->spaceobject == NULL)
            	{
            	    echo_to_cockpit( AT_RED, ship, "Ship lost in Hyperspace. Make new calculations.");
            	    return;
            	}
            	else
            	{
		    float tx, ty, tz;

		    for( spaceobject = first_spaceobject; spaceobject; spaceobject = spaceobject->next )
		      if( space_in_range( ship, spaceobject ) )
		      {
		        ship->currjump = spaceobject;
		        break;
		      }
		    if( !spaceobject )
		      ship->currjump = ship->spaceobject;

		    tx = ship->vx;
		    ty = ship->vy;
		    tz = ship->vz;

		    ship->vx = ship->cx;
		    ship->vy = ship->cy;
		    ship->vz = ship->cz;

		    ship->cx = tx;
		    ship->cy = ty;
		    ship->cz = tz;
		    ship->currjump = NULL;

            	    echo_to_room( AT_YELLOW, get_room_index(ship->pilotseat), "Hyperjump complete.");
            	    echo_to_ship( AT_YELLOW, ship, "The ship lurches slightly as it comes out of hyperspace.");
					// 2/18/04 - Johnson - Modified call to reflect origin of object entering the system
            	    //sprintf( buf ,"%s enters the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
					sprintf( buf ,"%s enters the starsystem from hyperspace at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
            	    echo_to_system( AT_YELLOW, ship, buf , NULL );
            	    ship->shipstate = SHIP_READY;
            	    STRFREE( ship->home );
            	    ship->home = STRALLOC( ship->spaceobject->name );
            	    if ( str_cmp("Public",ship->owner) )
                        save_ship(ship);

		    for( dship = first_ship; dship; dship = dship->next )
		      if ( dship->docked && dship->docked == ship )
		      {
            	        echo_to_room( AT_YELLOW, get_room_index(dship->pilotseat), "Hyperjump complete.");
            	        echo_to_ship( AT_YELLOW, dship, "The ship lurches slightly as it comes out of hyperspace.");
					// 2/18/04 - Johnson - Modified call to reflect origin of object entering the system
            	    //sprintf( buf ,"%s enters the starsystem at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
					sprintf( buf ,"%s enters the starsystem from hyperspace at %.0f %.0f %.0f" , ship->name, ship->vx, ship->vy, ship->vz );
            	        echo_to_system( AT_YELLOW, dship, buf , NULL );
            	        STRFREE( dship->home );
            	        dship->home = STRALLOC( ship->home );
            	        if ( str_cmp("Public",dship->owner) )
                          save_ship(dship);
		      }		      	  


                    return;

        	}
               }
        
			
                if (!ship->currjump)
    	        {
    	            send_to_char("&RYou need to calculate your jump first!\n\r",ch);
    	            return;
    	        } 

        if ( ship->energy < 100)
        {
              send_to_char("&RTheres not enough fuel!\n\r",ch);
              return;
        }

        if ( ship->currspeed <= 0 )
        {
              send_to_char("&RYou need to speed up a little first!\n\r",ch);
              return;
        }

	   
/*
    	for ( eShip = first_ship; eShip; eShip = eShip->next )
    	{
    	   if ( eShip == ship )
    	      continue;
 	   if ( !eShip->spaceobject )
 	      continue;
    	   if ( eShip->docked && eShip->docked == ship )
    	      continue;
    	   if ( abs( eShip->vx - ship->vx ) < 500 
    	   &&  abs( eShip->vy - ship->vy ) < 500 
    	   &&  abs( eShip->vz - ship->vz ) < 500 )
           {
              ch_printf(ch, "&RYou are too close to %s to make the jump to lightspeed.\n\r", eShip->name );
              return;
           }    	   
    	}
  Ships removed and space objects added to gravity wells preventing hyperspace - Darrik Vequir - 6/01

 */   	        

	for( spaceobject = first_spaceobject; spaceobject; spaceobject = spaceobject->next )
	{
	  if( ( abs ( (int) ( spaceobject->xpos - ship->vx ) ) < 100+(spaceobject->gravity*5) )
	   && ( abs ( (int) ( spaceobject->ypos - ship->vy ) ) < 100+(spaceobject->gravity*5) )
	   && ( abs ( (int) ( spaceobject->zpos - ship->vz ) ) < 100+(spaceobject->gravity*5) ) )
           {
              ch_printf(ch, "&RYou are too close to %s to make the jump to lightspeed.\n\r", spaceobject->name );
              return;
           }
        }

        if ( ship->shipclass == FIGHTER_SHIP )
             chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
        if ( ship->shipclass == MIDSIZE_SHIP )
             chance = IS_NPC(ch) ? ch->top_level
                 : (int)  (ch->pcdata->learned[gsn_midships]) ;

/* changed mobs so they can not fly capital ships. Forcers could possess mobs
   and fly them - Darrik Vequir */
        if ( ship->shipclass == CAPITAL_SHIP )
              chance = IS_NPC(ch) ? 0
                 : (int) (ch->pcdata->learned[gsn_capitalships]);
        if ( number_percent( ) >= chance )
        {
            send_to_char("&RYou can't figure out which lever to use.\n\r",ch);
            if ( ship->shipclass == FIGHTER_SHIP )
               learn_from_failure( ch, gsn_starfighters );
            if ( ship->shipclass == MIDSIZE_SHIP )
               learn_from_failure( ch, gsn_midships );
            if ( ship->shipclass == CAPITAL_SHIP )
                learn_from_failure( ch, gsn_capitalships );
    	   return;
        }
    sprintf( buf ,"%s enters hyperspace." , ship->name );
    echo_to_system( AT_YELLOW, ship, buf , NULL );

    ship->lastsystem = ship->spaceobject;
    ship_from_spaceobject( ship , ship->spaceobject );
    ship->shipstate = SHIP_HYPERSPACE;

    send_to_char( "&GYou push forward the hyperspeed lever.\n\r", ch);
    act( AT_PLAIN, "$n pushes a lever forward on the control panel.", ch,
         NULL, argument , TO_ROOM );
    echo_to_ship( AT_YELLOW , ship , "The ship lurches slightly as it makes the jump to lightspeed." );
    echo_to_cockpit( AT_YELLOW , ship , "The stars become streaks of light as you enter hyperspace.");
    echo_to_docked( AT_YELLOW , ship, "The stars become streaks of light as you enter hyperspace." );

    ship->energy -= 100;

    tx = ship->vx;
    ty = ship->vy;
    tz = ship->vz;

    ship->vx = ship->jx;
    ship->vy = ship->jy;
    ship->vz = ship->jz;

    ship->cx = tx;
    ship->cy = ty;
    ship->cz = tz;

    ship->jx = tx;
    ship->jy = ty;
    ship->jz = tz;

    ship->ox = tx;
    ship->oy = ty;
    ship->oz = tz;

    if ( ship->shipclass == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->shipclass == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->shipclass == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    	
}

    
void do_target(CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    SHIP_DATA *target, *dship;
    char buf[MAX_STRING_LENGTH];
    bool isturret = FALSE;
    TURRET_DATA *turret;

    strcpy( arg, argument );

    switch( ch->substate )
    {
    	default:
    	        if (  (ship = ship_from_turret(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the gunners seat or turret of a ship to do that!\n\r",ch);
    	            return;
    	        }
		if ( ship->gunseat != ch->in_room->vnum )
		  isturret = TRUE;
/*                if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
*/
                if (ship->shipstate == SHIP_HYPERSPACE && ship->shipclass <= SHIP_PLATFORM)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;
                }
    	        if (! ship->spaceobject && ship->shipclass <= SHIP_PLATFORM)
    	        {
    	            send_to_char("&RYou can't do that until you've finished launching!\n\r",ch);
    	            return;
    	        }

                if ( autofly(ship) && ( !isturret || !check_pilot( ch, ship ) ) )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first....\n\r",ch);
    	            return;
    	        }

                if (arg[0] == '\0')
    	        {
    	            send_to_char("&RYou need to specify a target!\n\r",ch);
    	            return;
    	        }

                if ( !str_cmp( arg, "none") )
    	        {
    	            send_to_char("&GTarget set to none.\n\r",ch);
    	            if ( ch->in_room->vnum == ship->gunseat )
    	                   ship->target0 = NULL;
    	            if( ship->first_turret )
    	              for ( turret = ship->first_turret; turret; turret = turret->next )
    	                if ( ch->in_room->vnum == turret->roomvnum )
    	                   turret->target = NULL;

    	            return;
    	        }
              if (ship->shipclass > SHIP_PLATFORM)
                target = ship_in_room( ship->in_room , arg );
              else
                target = get_ship_here( arg, ship );


                if (  target == NULL )
                {
                    send_to_char("&RThat ship isn't here!\n\r",ch);
                    return;
                }

                if (  target == ship )
                {
                    send_to_char("&RYou can't target your own ship!\n\r",ch);
                    return;
                }

                if ( !str_cmp(ship->owner, "Trainer") && str_cmp(target->owner, "Trainer") )
                {
                    send_to_char("&RTrainers can only target other trainers!!\n\r",ch);
                    return;
                }
                if ( str_cmp(ship->owner, "Trainer") && !str_cmp(target->owner, "Trainer") )
                {
                    send_to_char("&ROnly trainers can target other trainers!!\n\r",ch);
                    return;
                }
               if( ship->shipclass <= SHIP_PLATFORM)
               {
                if ( abs( (int) ( ship->vx-target->vx )) > 5000 ||
                     abs( (int) ( ship->vy-target->vy )) > 5000 ||
                     abs( (int) ( ship->vz-target->vz )) > 5000 )
                {
                    send_to_char("&RThat ship is too far away to target.\n\r",ch);
                    return;
                }
               }

                chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_weaponsystems]) ;
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GTracking target.\n\r", ch);
    		   act( AT_PLAIN, "$n makes some adjustments on the targeting computer.", ch,
		        NULL, argument , TO_ROOM );
    		   add_timer ( ch , TIMER_DO_FUN , 1 , do_target , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou fail to work the controls properly.\n\r",ch);
	        learn_from_failure( ch, gsn_weaponsystems );
    	   	return;

    	case 1:
    		if ( !ch->dest_buf )
    		   return;
    		strcpy(arg, ( const char* ) ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		break;

    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		ch->substate = SUB_NONE;
    		if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    		      return;
    	        send_to_char("&RYour concentration is broken. You fail to lock onto your target.\n\r", ch);
    		return;
    }

    ch->substate = SUB_NONE;

    if ( (ship = ship_from_turret(ch->in_room->vnum)) == NULL )
    {
       return;
    }
   if (ship->shipclass > SHIP_PLATFORM)
    target = ship_in_room( ship->in_room , arg );
   else
    target = get_ship_here( arg, ship );

    if (  target == NULL || target == ship)
    {
           send_to_char("&RThe ship has left the starsytem. Targeting aborted.\n\r",ch);
           return;
    }

    if ( ch->in_room->vnum == ship->gunseat )
       ship->target0 = target;

    if( ship->first_turret )
      for( turret = ship->first_turret; turret; turret = turret->next )
        if ( ch->in_room->vnum == turret->roomvnum )
          turret->target = target;


    send_to_char( "&GTarget Locked.\n\r", ch);
    sprintf( buf , "You are being targetted by %s." , ship->name);  
    echo_to_cockpit( AT_BLOOD , target , buf );
    echo_to_docked( AT_YELLOW , ship, "The ship's computer receives targetting data through the docking port link." );

    if ( ch->in_room->vnum == ship->gunseat )
     for( dship = first_ship; dship; dship = dship->next )
      if( dship->docked && dship->docked == ship )
        dship->target0 = target;
       	      
    sound_to_room( ch->in_room , "!!SOUND(targetlock)" );
    learn_from_success( ch, gsn_weaponsystems );
    	
    if ( autofly(target) && !target->target0)
    {
       sprintf( buf , "You are being targetted by %s." , target->name);  
       echo_to_cockpit( AT_BLOOD , ship , buf );
       target->target0 = ship;
    }
}

int calc_manuever( SHIP_DATA *ship )
{
  int manuever;
  int weight;
  
  manuever = ship->mod->manuever;
  manuever *= 10;
  
  weight = (ship->weight/5);
  if( !weight )
    weight = 300*(ship->shipclass+1);
  
  weight += (ship->mod->maxhull/5)+(ship->energy/40);
  if( get_extmodule_count(ship) > 5 )
    weight += (ship->maxextmodules - get_extmodule_count(ship))*-20;
  
  if( weight <= 0 )
    weight = 300*(ship->shipclass+1);
  
  manuever *= 400;
  manuever /= (weight+1)*3;
  
  return manuever;
}

int calc_hit( SHIP_DATA *ship, SHIP_DATA *target )
{
  int distance = 0, chance = 0;
  int shipmanuever;
  int targetmanuever;
  int chancesize = 0;
  int chanceclass = 0;
  int chancespeed = 0;
  int chancemanuever = 0;
  int chancedistance = 0;
  int chancedivisor = 0;
  
  shipmanuever = calc_manuever( ship );
  targetmanuever = calc_manuever( target );
  
  distance = abs( (int) ( target->vx - ship->vx )) 
           + abs( (int) ( target->vy - ship->vy )) 
 	       + abs( (int) ( target->vz - ship->vz ));
  distance /= 3;

  chancesize -= (ship->weight+(ship->mod->maxhull/5));
  chancesize += (target->weight+(target->mod->maxhull/5));
  chancesize /= 200;
  
  chanceclass = ( target->shipclass - ship->shipclass ) *32;
  
  chancespeed = -1*abs((ship->currspeed - target->currspeed)/2);
  
  chancemanuever = ( shipmanuever-targetmanuever )/10;
  
  chancedistance -= distance/(10*(target->shipclass+1));
  
  chance = chancesize+chanceclass+chancespeed+chancemanuever+chancedistance;
  
  if ( chancesize )
    chancedivisor++;
  if ( chanceclass )
    chancedivisor++;
  if ( chancespeed )
    chancedivisor++;
  if ( chancemanuever )
    chancedivisor++;
  if ( chancedistance )
    chancedivisor++;
  
  chancedivisor--;
  
  chance /= chancedivisor;
  
  chance = URANGE( -49 , chance , 49 );

  return chance;
}

void do_fire(CHAR_DATA *ch, char *argument )
{
    int chance, origchance, distance;
    SHIP_DATA *ship;
    SHIP_DATA *target;
    char buf[MAX_STRING_LENGTH];
    bool isturret = FALSE;    
    TURRET_DATA *turret;
    bool destroyed;
    
        if (  (ship = ship_from_turret(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the gunners chair or turret of a ship to do that!\n\r",ch);
            return;
        }
		if ( ship->gunseat != ch->in_room->vnum )
		  isturret = TRUE;
        
  /*      if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
  */  	        
        if (ship->shipstate == SHIP_HYPERSPACE && ship->shipclass <= SHIP_PLATFORM)
        {
             send_to_char("&RYou can only do that in realspace!\n\r",ch);
             return;   
        }
    	if (ship->spaceobject == NULL && ship->shipclass <= SHIP_PLATFORM)
    	{
    	     send_to_char("&RYou can't do that until after you've finished launching!\n\r",ch);
    	     return;
    	}
    	if ( ship->energy <5 )
        {
             send_to_char("&RTheres not enough energy left to fire!\n\r",ch);
             return;
        }   
  
                if ( autofly(ship) && !isturret )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first.\n\r",ch);
    	            return;
    	        }
    	              
        
        chance = IS_NPC(ch) ? ch->top_level
                 : (int) ( ch->perm_dex*2 + ch->pcdata->learned[gsn_spacecombat]/3
                           + ch->pcdata->learned[gsn_spacecombat2]/3 + ch->pcdata->learned[gsn_spacecombat3]/3 );        
        origchance = chance;

        if ( ship->shipclass > SHIP_PLATFORM && !IS_NPC(ch))
          ((ch->pcdata->learned[gsn_speeders] == 100) ? (chance -= 100 - ch->pcdata->learned[gsn_speedercombat]) : (chance = 0));
        
    	if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "lasers") )
    	{
    	
    	     if (ship->statet0 == LASER_DAMAGED)
    	     {
    	        send_to_char("&RThe ships main laser is damaged.\n\r",ch);
    	      	return;
    	     } 
             if (ship->statet0 >= ship->mod->lasers )
    	     {
    	     	send_to_char("&RThe lasers are still recharging.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
    	     if (ship->shipclass <= SHIP_PLATFORM && !ship_in_range( ship, target ) )
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     } 
    	     if (ship->shipclass > SHIP_PLATFORM && ship->target0->in_room != ship->in_room)
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     } 
            if(ship->shipclass <= SHIP_PLATFORM)
            {
             if ( abs( (int) ( target->vx - ship->vx )) >1000 ||
                  abs( (int) ( target->vy - ship->vy )) >1000 ||
                  abs( (int) ( target->vz - ship->vz )) >1000 )
             {
                send_to_char("&RThat ship is out of laser range.\n\r",ch);
    	     	return;
             } 
            }
             if ( ship->shipclass < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RThe main laser can only fire forward. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }      
             ship->statet0++;

	     chance /= 2;
	     chance += calc_hit( ship, target );
	     
             chance = URANGE( 1 , chance , 99 );

             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             if ( number_percent( ) > chance )
             {  
                sprintf( buf , "Lasers fire from %s at you but miss." , ship->name);  
                echo_to_cockpit( AT_ORANGE , target , buf );           
                sprintf( buf , "The ships lasers fire at %s but miss." , target->name);
                echo_to_cockpit( AT_ORANGE , ship , buf );
                learn_from_failure( ch, gsn_spacecombat );
    	        learn_from_failure( ch, gsn_spacecombat2 );
    	        learn_from_failure( ch, gsn_spacecombat3 );
    	        sprintf( buf, "Laserfire from %s barely misses %s." , ship->name , target->name );
           if(ship->shipclass > SHIP_PLATFORM)
             echo_to_room(AT_ORANGE, ship->in_room, buf);
           else
                echo_to_system( AT_ORANGE , ship , buf , target );
    	        return;
             }
             sprintf( buf, "Laserfire from %s hits %s." , ship->name, target->name );
           if(ship->shipclass > SHIP_PLATFORM)
             echo_to_room(AT_ORANGE, ship->in_room, buf);
           else
             echo_to_system( AT_ORANGE , ship , buf , target );
             sprintf( buf , "You are hit by lasers from %s!" , ship->name);
             echo_to_cockpit( AT_BLOOD , target , buf );
             sprintf( buf , "Your ships lasers hit %s!." , target->name);
             echo_to_cockpit( AT_YELLOW , ship , buf );
             learn_from_success( ch, gsn_spacecombat );
             learn_from_success( ch, gsn_spacecombat2 );
             learn_from_success( ch, gsn_spacecombat3 );
           if (ship->shipclass > SHIP_PLATFORM )
             learn_from_success( ch, gsn_speedercombat );
           
             echo_to_ship( AT_RED , target , "A small explosion vibrates through the ship." );

             if ( autofly(target) && target->target0 != ship && ship->spaceobject)
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
	    if( ship->shipclass == SHIP_PLATFORM )
	      destroyed = damage_ship_ch( target, 100, 250, ch);
	    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass < CAPITAL_SHIP )
	      destroyed = damage_ship_ch( target, 50, 200, ch );
	    else 
              destroyed = damage_ship_ch( target, 10 , 50, ch );
             
	    if ( destroyed )
	      ship->target0 = NULL;    
             
             return;
    	}

/*************************
 *  Added ion cannons!   *
 *  Darrik Vequir        *
 *************************/

    	if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "ions") )
    	{

    	     if (ship->statet0 == LASER_DAMAGED)
    	     {
    	        send_to_char("&RThe ships main weapons are damaged.\n\r",ch);
    	      	return;
    	     }
    	     if (ship->mod->ions <= 0)
    	     {
    	     	send_to_char("&RYou have no ion cannons to fire.\n\r", ch);
    	     	return;
    	     }
             if (ship->statei0 >= ship->mod->ions )
    	     {
    	     	send_to_char("&RThe ion cannons are still recharging.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }
    	     target = ship->target0;
    	     if (ship->shipclass <= SHIP_PLATFORM && !ship_in_range(ship, target) )
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL;
    	     	return;
    	     }
    	    if (ship->shipclass <= SHIP_PLATFORM)
    	    {
             if ( abs( (int) ( target->vx - ship->vx )) >1000 ||
                  abs( (int) ( target->vy - ship->vy )) >1000 ||
                  abs( (int) ( target->vz - ship->vz )) >1000 )
             {
                send_to_char("&RThat ship is out of ion range.\n\r",ch);
    	     	return;
             }
            }
             if ( ship->shipclass < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RThe main ion cannon can only fire forward. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }
             ship->statei0++;

             distance = abs( (int) ( target->vx - ship->vx )) 
               + abs( (int) ( target->vy - ship->vy )) 
               + abs( (int) ( target->vz - ship->vz ));
	     distance /= 3;
	     chance += target->shipclass - ship->shipclass;
	     chance += ship->currspeed - target->currspeed;
	     chance += ship->mod->manuever - target->mod->manuever;
	     chance -= distance/(10*(target->shipclass+1));
	     chance -= origchance;
	     chance /= 2;
	     chance += origchance;
             chance = URANGE( 1 , chance , 99 );

             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             if ( number_percent( ) > chance )
             {
                sprintf( buf , "Ion cannons fire from %s at you, but the blue plasma narrowly misses." , ship->name);
                echo_to_cockpit( AT_ORANGE , target , buf );
                sprintf( buf , "The ships ion cannons fire at %s but the blue plasma narrowly misses." , target->name);
                echo_to_cockpit( AT_ORANGE , ship , buf );
                learn_from_failure( ch, gsn_spacecombat );
    	        learn_from_failure( ch, gsn_spacecombat2 );
    	        learn_from_failure( ch, gsn_spacecombat3 );
    	        sprintf( buf, "Blue ion plasma from %s narrowly misses %s." , ship->name , target->name );
                if(ship->shipclass > SHIP_PLATFORM)
                  echo_to_room(AT_ORANGE, ship->in_room, buf);
                else
                  echo_to_system( AT_ORANGE , ship , buf , target );
    	        return;
             }
             sprintf( buf, "Blue plasma from %s engulfs %s." , ship->name, target->name );
           if(ship->shipclass > SHIP_PLATFORM)
             echo_to_room(AT_ORANGE, ship->in_room, buf);
           else
             echo_to_system( AT_ORANGE , ship , buf , target );
             sprintf( buf , "You are engulfed by ion energy from %s!" , ship->name);
             echo_to_cockpit( AT_BLOOD , target , buf );
             sprintf( buf , "Blue plasma from your ship engulf %s!." , target->name);
             echo_to_cockpit( AT_YELLOW , ship , buf );
             learn_from_success( ch, gsn_spacecombat );
             learn_from_success( ch, gsn_spacecombat2 );
             learn_from_success( ch, gsn_spacecombat3 );
           if (ship->shipclass > SHIP_PLATFORM )
             learn_from_success( ch, gsn_speedercombat );
             
             echo_to_ship( AT_RED , target , "A small explosion vibrates through the ship." );
             if ( target && autofly(target) && target->target0 != ship && ship->spaceobject)
             {
                target->target0 = ship;
                sprintf( buf , "You are being targetted by %s." , target->name);
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }

	    if( ship->shipclass == SHIP_PLATFORM )
	      destroyed = damage_ship_ch( target, -200, -50, ch );
	    else if( ship->shipclass == CAPITAL_SHIP && target->shipclass <= MIDSIZE_SHIP )
	      destroyed = damage_ship_ch( target, -200, -50, ch );
	    else 
              destroyed = damage_ship_ch( target, -75 , -10, ch );

	    if ( destroyed )
	      ship->target0 = NULL;

             return;
    	}


/*************************
 *   End ions addition   *
 *************************/

        if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "missile") )
    	{
    	     if (ship->missilestate == MISSILE_DAMAGED)
    	     {
    	        send_to_char("&RThe ships missile launchers are dammaged.\n\r",ch);
    	      	return;
    	     } 
             if (ship->missiles <= 0)
    	     {
    	     	send_to_char("&RYou have no missiles to fire!\n\r",ch);
    	        return;
    	     }
    	     if (ship->missilestate != MISSILE_READY )
    	     {
    	     	send_to_char("&RThe missiles are still reloading.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
             if (ship->shipclass <= SHIP_PLATFORM && !ship_in_range( ship, target) )
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     } 
    	    if(ship->shipclass <= SHIP_PLATFORM)
    	    {
             if ( abs( (int) ( target->vx - ship->vx )) >1000 ||
                  abs( (int) ( target->vy - ship->vy )) >1000 ||
                  abs( (int) ( target->vz - ship->vz )) >1000 )
             {
                send_to_char("&RThat ship is out of missile range.\n\r",ch);
    	     	return;
             }
            } 
             if ( ship->shipclass < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RMissiles can only fire in a forward. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }      

             distance = abs( (int) ( target->vx - ship->vx )) 
               + abs( (int) ( target->vy - ship->vy )) 
               + abs( (int) ( target->vz - ship->vz ));
	     distance /= 3;
	     chance += target->shipclass - ship->shipclass;
	     chance += ship->currspeed - target->currspeed;
	     chance += ship->mod->manuever - target->mod->manuever;
	     chance -= distance/(10*(target->shipclass+1));
	     chance -= origchance;
	     chance /= 2;
	     chance += origchance;

	     chance += 30;

             chance = URANGE( 20 , chance , 99 );

             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             if ( number_percent( ) > chance )
             {
                send_to_char( "&RYou fail to lock onto your target!", ch );
                ship->missilestate = MISSILE_RELOAD_2;
    	        return;	
             } 
             ship->missiles-- ;
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             echo_to_cockpit( AT_YELLOW , ship , "Missiles launched.");
             sprintf( buf , "Incoming missile from %s." , ship->name);  
           if(ship->shipclass > SHIP_PLATFORM)
             echo_to_ship( AT_RED , target , "A large explosion vibrates through the ship." );

             echo_to_cockpit( AT_BLOOD , target , buf );
             sprintf( buf, "%s fires a missile towards %s." , ship->name, target->name );
           if(ship->shipclass > SHIP_PLATFORM)
             echo_to_room(AT_ORANGE, ship->in_room, buf);
           else
             echo_to_system( AT_ORANGE , ship , buf , target );
             learn_from_success( ch, gsn_weaponsystems );
             if ( ship->shipclass == CAPITAL_SHIP || ship->shipclass == SHIP_PLATFORM )
                   ship->missilestate = MISSILE_RELOAD;
             else
                   ship->missilestate = MISSILE_FIRED;
             
             if ( autofly(target) && target->target0 != ship && ship->spaceobject)
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
           if(ship->shipclass <= SHIP_PLATFORM)
             new_missile( ship , target , ch , CONCUSSION_MISSILE );
           else
             destroyed = damage_ship_ch(target, 75, 200, ch );

	   if ( destroyed )
	     ship->target0 = NULL;

             return;
    	}
        if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "torpedo") )
    	{
    	     if (ship->missilestate == MISSILE_DAMAGED)
    	     {
    	        send_to_char("&RThe ships missile launchers are dammaged.\n\r",ch);
    	      	return;
    	     } 
             if (ship->torpedos <= 0)
    	     {
    	     	send_to_char("&RYou have no torpedos to fire!\n\r",ch);
    	        return;
    	     }
    	     if (ship->missilestate != MISSILE_READY )
    	     {
    	     	send_to_char("&RThe torpedos are still reloading.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
             if (ship->shipclass <= SHIP_PLATFORM && !ship_in_range( ship, target) )
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     }
    	    if(ship->shipclass <= SHIP_PLATFORM)
    	    { 
             if ( abs( (int) ( target->vx - ship->vx )) >1000 ||
                  abs( (int) ( target->vy - ship->vy )) >1000 ||
                  abs( (int) ( target->vz - ship->vz )) >1000 )
             {
                send_to_char("&RThat ship is out of torpedo range.\n\r",ch);
    	     	return;
             }
            } 
             if ( ship->shipclass < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RTorpedos can only fire in a forward direction. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }      
             distance = abs( (int) ( target->vx - ship->vx )) 
               + abs( (int) ( target->vy - ship->vy )) 
               + abs( (int) ( target->vz - ship->vz ));
	     distance /= 3;
	     chance += target->shipclass - ship->shipclass;
	     chance += ship->currspeed - target->currspeed;
	     chance += ship->mod->manuever - target->mod->manuever;
	     chance -= distance/(10*(target->shipclass+1));
	     chance -= origchance;
	     chance /= 2;
	     chance += origchance;

	     chance += 30;

             chance = URANGE( 20 , chance , 99 );

             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             if ( number_percent( ) > chance )
             {
                send_to_char( "&RYou fail to lock onto your target!", ch );
                ship->missilestate = MISSILE_RELOAD_2;
    	        return;	
             } 
             ship->torpedos-- ;
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             echo_to_cockpit( AT_YELLOW , ship , "Missiles launched.");
             sprintf( buf , "Incoming torpedo from %s." , ship->name);  
             echo_to_cockpit( AT_BLOOD , target , buf );
             sprintf( buf, "%s fires a torpedo towards %s." , ship->name, target->name );
           if(ship->shipclass > SHIP_PLATFORM)
           {
             echo_to_room(AT_ORANGE, ship->in_room, buf);
             echo_to_ship( AT_RED , target , "A large explosion vibrates through the ship." );
           }
           else
             echo_to_system( AT_ORANGE , ship , buf , target );
             learn_from_success( ch, gsn_weaponsystems );
             if ( ship->shipclass == CAPITAL_SHIP || ship->shipclass == SHIP_PLATFORM )
                   ship->missilestate = MISSILE_RELOAD;
             else
                   ship->missilestate = MISSILE_FIRED;
             
             if ( autofly(target) && target->target0 != ship && ship->spaceobject)
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
           if( ship->shipclass <= SHIP_PLATFORM)
             new_missile( ship , target , ch , PROTON_TORPEDO );
           else
             damage_ship_ch( target, 200, 300, ch);

             return;
    	}
    	
        if ( ch->in_room->vnum == ship->gunseat && !str_prefix( argument , "rocket") )
    	{
    	     if (ship->missilestate == MISSILE_DAMAGED)
    	     {
    	        send_to_char("&RThe ships missile launchers are damaged.\n\r",ch);
    	      	return;
    	     } 
             if (ship->rockets <= 0)
    	     {
    	     	send_to_char("&RYou have no rockets to fire!\n\r",ch);
    	        return;
    	     }
    	     if (ship->missilestate != MISSILE_READY )
    	     {
    	     	send_to_char("&RThe missiles are still reloading.\n\r",ch);
    	     	return;
    	     }
    	     if (ship->target0 == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = ship->target0;
             if (ship->shipclass <= SHIP_PLATFORM && !ship_in_range( ship, target) )
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        ship->target0 = NULL; 
    	     	return;
    	     }
    	    if (ship->shipclass <= SHIP_PLATFORM)
    	    { 
             if ( abs( (int) ( target->vx - ship->vx )) >800 ||
                  abs( (int) ( target->vy - ship->vy )) >800 ||
                  abs( (int) ( target->vz - ship->vz )) >800 )
             {
                send_to_char("&RThat ship is out of rocket range.\n\r",ch);
    	     	return;
             }
            } 
             if ( ship->shipclass < 2 && !is_facing( ship, target ) )
             {
                send_to_char("&RRockets can only fire forward. You'll need to turn your ship!\n\r",ch);
    	     	return;
             }      

             distance = abs( (int) ( target->vx - ship->vx )) 
               + abs( (int) ( target->vy - ship->vy )) 
               + abs( (int) ( target->vz - ship->vz ));
	     distance /= 3;
	     chance += target->shipclass - ship->shipclass;
	     chance += ship->currspeed - target->currspeed;
	     chance += ship->mod->manuever - target->mod->manuever;
	     chance -= distance/(10*(target->shipclass+1));
	     chance -= origchance;
	     chance /= 2;
	     chance += origchance;

	     chance += 30;

             chance = URANGE( 20 , chance , 99 );

             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             if ( number_percent( ) > chance )
             {
                send_to_char( "&RYou fail to lock onto your target!", ch );
                ship->missilestate = MISSILE_RELOAD_2;
    	        return;	
             } 

             ship->rockets-- ;
             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );
             echo_to_cockpit( AT_YELLOW , ship , "Rocket launched.");
             sprintf( buf , "Incoming rocket from %s." , ship->name);  
             echo_to_cockpit( AT_BLOOD , target , buf );
             sprintf( buf, "%s fires a heavy rocket towards %s." , ship->name, target->name );
           if(ship->shipclass > SHIP_PLATFORM)
           {
             echo_to_room(AT_ORANGE, ship->in_room, buf);
             echo_to_ship( AT_RED , target , "A large explosion vibrates through the ship." );
           }
           else
             echo_to_system( AT_ORANGE , ship , buf , target );
             learn_from_success( ch, gsn_weaponsystems );
             if ( ship->shipclass == CAPITAL_SHIP || ship->shipclass == SHIP_PLATFORM )
                   ship->missilestate = MISSILE_RELOAD;
             else
                   ship->missilestate = MISSILE_FIRED;
             
             if ( autofly(target) && target->target0 != ship && ship->spaceobject)
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
           if( ship->shipclass <= SHIP_PLATFORM)
             new_missile( ship , target , ch , HEAVY_ROCKET );
           else
             damage_ship_ch( target, 450, 550, ch );
             
             return;
    	}
        
        if( ship->first_turret )
          for( turret = ship->first_turret; turret; turret = turret->next )
            if ( ch->in_room->vnum == turret->roomvnum && !str_prefix( argument , "lasers") )
    	    {
    	     if (turret->state == LASER_DAMAGED)
    	     {
    	        send_to_char("&RThe ships turret is damaged.\n\r",ch);
    	      	return;
    	     } 
             if (turret->state > ship->shipclass )
    	     {
    	     	send_to_char("&RThe turret is recharging.\n\r",ch);
    	     	return;
    	     }
    	     if (turret->target == NULL )
    	     {
    	     	send_to_char("&RYou need to choose a target first.\n\r",ch);
    	     	return;
    	     }    	    
    	     target = turret->target;
             if (ship->shipclass <= SHIP_PLATFORM && !ship_in_range( ship, target) )
    	     {
    	     	send_to_char("&RYour target seems to have left.\n\r",ch);
    	        turret->target = NULL; 
    	     	return;
    	     }
    	    if (ship->shipclass <= SHIP_PLATFORM)
    	    { 
             if ( abs( (int) ( target->vx - ship->vx )) >1000 ||
                  abs( (int) ( target->vy - ship->vy )) >1000 ||
                  abs( (int) ( target->vz - ship->vz )) >1000 )
             {
                send_to_char("&RThat ship is out of range.\n\r",ch);
    	     	return;
             }
            } 
             turret->state++;

             distance = abs( (int) ( target->vx - ship->vx )) 
               + abs( (int) ( target->vy - ship->vy )) 
               + abs( (int) ( target->vz - ship->vz ));
	     distance /= 3;
	     chance += target->shipclass - CAPITAL_SHIP+1;
	     chance += ship->currspeed - target->currspeed;
	     chance += ( 100 + ( turret->type * 50 ) ) - target->mod->manuever;
	     chance -= distance/(10*(target->shipclass+1));
	     chance -= origchance;
	     chance /= 2;
	     chance += origchance;
             chance = URANGE( 1 , chance , 99 );

             act( AT_PLAIN, "$n presses the fire button.", ch,
                  NULL, argument , TO_ROOM );

             if ( number_percent( ) > chance )
             {  
                sprintf( buf , turretstats[turret->type].targetmissmsg , ship->name);  
                echo_to_cockpit( AT_ORANGE , target , buf );           
                sprintf( buf , turretstats[turret->type].selfmissmsg , target->name);  
                echo_to_cockpit( AT_ORANGE , ship , buf );           
                sprintf( buf , turretstats[turret->type].observermissmsg , ship->name, target->name );
           if(ship->shipclass > SHIP_PLATFORM)
             echo_to_room(AT_ORANGE, ship->in_room, buf);
           else
                echo_to_system( AT_ORANGE , ship , buf , target );
                learn_from_failure( ch, gsn_spacecombat ); 
    	        learn_from_failure( ch, gsn_spacecombat2 ); 
    	        learn_from_failure( ch, gsn_spacecombat3 ); 
    	        return;	
             } 
                sprintf( buf , turretstats[turret->type].targetdammsg , ship->name);  
                echo_to_cockpit( AT_BLOOD , target , buf );           
                sprintf( buf , turretstats[turret->type].selfdammsg , target->name);  
                echo_to_cockpit( AT_YELLOW , ship , buf );           
                sprintf( buf , turretstats[turret->type].observerdammsg , ship->name, target->name );
           if(ship->shipclass > SHIP_PLATFORM)
             echo_to_room(AT_ORANGE, ship->in_room, buf);
           else
                echo_to_system( AT_ORANGE , ship , buf , target );
                learn_from_success( ch, gsn_spacecombat );
                learn_from_success( ch, gsn_spacecombat2 );
                learn_from_success( ch, gsn_spacecombat3 );
                echo_to_ship( AT_RED , target , "A small explosion vibrates through the ship." );           

	    damage_ship_ch( target, turretstats[turret->type].mindamage, turretstats[turret->type].maxdamage, ch );

/*	    if( ship->shipclass == SHIP_PLATFORM && target->shipclass <= MIDSIZE_SHIP )
	      damage_ship_ch( target, 100, 250, ch);
	    else if ( target->shipclass <= MIDSIZE_SHIP )
	      damage_ship_ch( target, 50, 200, ch );
	    else 
              damage_ship_ch( target, 10 , 50, ch );
*/
/*
             if ( number_percent( ) > chance )
             {  
                sprintf( buf , "Turbolasers fire from %s at you but miss." , ship->name);  
                echo_to_cockpit( AT_ORANGE , target , buf );           
                sprintf( buf , "Turbolasers fire from the ships turret at %s but miss." , target->name);  
                echo_to_cockpit( AT_ORANGE , ship , buf );           
                sprintf( buf , "The turbo-laser fires from %s at %s and misses." , ship->name, target->name );
           if(ship->shipclass > SHIP_PLATFORM)
             echo_to_room(AT_ORANGE, ship->in_room, buf);
           else
                echo_to_system( AT_ORANGE , ship , buf , target );
                learn_from_failure( ch, gsn_spacecombat ); 
    	        learn_from_failure( ch, gsn_spacecombat2 ); 
    	        learn_from_failure( ch, gsn_spacecombat3 ); 
    	        return;	
             } 

           sprintf( buf, "Turboasers fire from %s, hitting %s." , ship->name, target->name );
           if(ship->shipclass > SHIP_PLATFORM)
             echo_to_room(AT_ORANGE, ship->in_room, buf);
           else
             echo_to_system( AT_ORANGE , ship , buf , target );
             sprintf( buf , "You are hit by turbolasers from %s!" , ship->name);  
             echo_to_cockpit( AT_BLOOD , target , buf );           
             sprintf( buf , "Turbolasers fire from the turret, hitting %s!." , target->name);  
             echo_to_cockpit( AT_YELLOW , ship , buf );           
             learn_from_success( ch, gsn_spacecombat );
             learn_from_success( ch, gsn_spacecombat2 );
             learn_from_success( ch, gsn_spacecombat3 );
             echo_to_ship( AT_RED , target , "A small explosion vibrates through the ship." );           
*/
/*	    if( ship->shipclass == SHIP_PLATFORM && target->shipclass <= MIDSIZE_SHIP )
	      damage_ship_ch( target, 100, 250, ch);
	    else if ( target->shipclass <= MIDSIZE_SHIP )
	      damage_ship_ch( target, 50, 200, ch );
	    else 
              damage_ship_ch( target, 10 , 50, ch );
*/             
             if ( autofly(target) && target->target0 != ship && ship->spaceobject)
             {
                target->target0 = ship; 
                sprintf( buf , "You are being targetted by %s." , target->name);  
                echo_to_cockpit( AT_BLOOD , ship , buf );
             }
             
             return;
    	    }
        


        send_to_char( "&RYou can't fire that!\n\r" , ch);

}


void do_calculate(CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];  
    char buf[MAX_INPUT_LENGTH];  
    int chance , distance = 0;
    SHIP_DATA *ship;
    SPACE_DATA *spaceobj, *spaceobject;
    bool found = FALSE;
    
    argument = one_argument( argument , arg1);
    argument = one_argument( argument , arg2);
    argument = one_argument( argument , arg3);
    

    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
                if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
                if (  (ship = ship_from_navseat(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be at a nav computer to calculate jumps.\n\r",ch);
    	            return;
    	        }

                if ( autofly(ship)  )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first....\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RAnd what exactly are you going to calculate...?\n\r" , ch );
                   return;
                }   
    	        if (ship->mod->hyperspeed == 0)
                {
                  send_to_char("&RThis ship is not equipped with a hyperdrive!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_LANDED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->spaceobject == NULL)
    	        {
    	            send_to_char("&RYou can only do that in realspace.\n\r",ch);
    	            return;
    	        }
    	        if (arg1[0] == '\0')
    	        {
    	            send_to_char("&WFormat: Calculate <spaceobject> <entry x> <entry y> <entry z>\n\r&wPossible destinations:\n\r",ch);
/*    	            for ( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
                    {
                       set_char_color( AT_NOTE, ch );
                       ch_printf(ch,"%-30s %d\n\r",spaceobj->name,
                                 (abs(spaceobj->xpos - ship->spaceobj->xpos)+
                                 abs(spaceobj->ypos - ship->spaceobj->ypos))/2);
                       count++;
                    }
                    if ( !count )
                    {
                        send_to_char( "No spacial objects found.\n\r", ch );
                    }
*/                  return;
    	        }
                chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_navigation]) ;
                if ( number_percent( ) > chance )
    		{
	           send_to_char("&RYou cant seem to figure the charts out today.\n\r",ch);
	           learn_from_failure( ch, gsn_navigation );
    	   	   return;
    	   	}

    if( !is_number(arg1) && arg1[0] != '-')
    {
      ship->currjump = spaceobject_from_name( arg1 );
      if ( arg2 && arg2[0] != '\0' )
        distance = atoi(arg2);
      if( ship->currjump )
      {
        ship->jx = ship->currjump->xpos;
        ship->jy = ship->currjump->ypos;
        ship->jz = ship->currjump->zpos;
        found = TRUE;
      }
    }  
    else if( arg2 && arg3 && arg2[0] != '\0' && arg2[0] != '\0' )
    {
    	ship->jx = atoi(arg1);
    	ship->jy = atoi(arg2);
    	ship->jz = atoi(arg3);
    	found = TRUE;
    }
    else
    {
      send_to_char("&WFormat: Calculate <spaceobject> \n\r        Calculate <entry x> <entry y> <entry z>&R&w\n\r",ch);
      return;
    }    

        spaceobject = ship->currjump;
           
        if ( !found )
        {
          send_to_char( "&RYou can't seem to find that spacial object on your charts.\n\r", ch);
          ship->currjump = NULL;
          return;
        }
        if (spaceobject && spaceobject->trainer && (ship->shipclass != SHIP_TRAINER))
        {
          send_to_char( "&RYou can't seem to find that spacial object on your charts.\n\r", ch);
          ship->currjump = NULL;
          return;
        }
        if (ship->shipclass == SHIP_TRAINER && spaceobject && !spaceobject->trainer )
        {
          send_to_char( "&RYou can't seem to find that starsytem on your charts.\n\r", ch);
          ship->currjump = NULL;
          return;
        }
             ship->jx += number_range ( ((ship->mod->astro_array) - 300) , (300-(ship->mod->astro_array)) );
             ship->jy += number_range ( ((ship->mod->astro_array) - 300) , (300-(ship->mod->astro_array)) );
             ship->jz += number_range ( ((ship->mod->astro_array) - 300) , (300-(ship->mod->astro_array)) );
             ship->jx += (distance ? distance : (spaceobject && spaceobject->gravity ? spaceobject->gravity*5 : 0 ) );
             ship->jy += (distance ? distance : (spaceobject && spaceobject->gravity ? spaceobject->gravity*5 : 0 ) );
             ship->jz += (distance ? distance : (spaceobject && spaceobject->gravity ? spaceobject->gravity*5 : 0 ) );

 	for ( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
          if ( !spaceobj->trainer && distance && strcmp(spaceobj->name,"") && 
                     abs( (int) ( ship->jx - spaceobj->xpos )) < (spaceobj->gravity*4) && 
                     abs( (int) ( ship->jy - spaceobj->ypos )) < (spaceobj->gravity*4) &&
                     abs( (int) ( ship->jz - spaceobj->zpos )) < (spaceobj->gravity*4) )
                {
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Jump coordinates too close to stellar object.");
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Hyperjump NOT set.");
                    ship->currjump = NULL;
                    return;
                }              

    for( spaceobject = first_spaceobject; spaceobject; spaceobject = spaceobject->next )
      if( space_in_range( ship, spaceobject ) )
      {
        ship->currjump = spaceobject;
        break;
      }
    if( !spaceobject )
      ship->currjump = ship->spaceobject;

    if( ship->jx > MAX_COORD_S || ship->jy > MAX_COORD_S || ship->jz > MAX_COORD_S ||
    ship->jx < -MAX_COORD_S || ship->jy < -MAX_COORD_S || ship->jz < -MAX_COORD_S )
    {
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Jump coordinates outside of the known galaxy.");
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Hyperjump NOT set.");
                    ship->currjump = NULL;
                    return;
    }
    
    ship->hyperdistance  = abs( (int) ( ship->vx - ship->jx )) ;
    ship->hyperdistance += abs( (int) ( ship->vy - ship->jy )) ;
    ship->hyperdistance += abs( (int) ( ship->vz - ship->jz )) ;
    ship->hyperdistance /= 200;

    if (ship->hyperdistance<100)
      ship->hyperdistance = 100;
      
    ship->orighyperdistance = ship->hyperdistance;

    sound_to_room( ch->in_room , "!!SOUND(computer)" );

    sprintf(buf, "&GHyperspace course set. Estimated distance: %d\n\rReady for the jump to lightspeed.\n\r", ship->hyperdistance );
    send_to_char( buf, ch);
    echo_to_docked( AT_YELLOW , ship, "The docking port link shows a new course being calculated." );

    act( AT_PLAIN, "$n does some calculations using the ships computer.", ch,
		        NULL, argument , TO_ROOM );
	                
    learn_from_success( ch, gsn_navigation );

    WAIT_STATE( ch , 2*PULSE_VIOLENCE );	
}

void do_calculate_diff(CHAR_DATA *ch, char *argument )
{
    char arg1[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char arg3[MAX_INPUT_LENGTH];  
    char buf[MAX_INPUT_LENGTH];  
    int chance , distance = 0;
    SHIP_DATA *ship;
    SPACE_DATA *spaceobj, *spaceobject;
    bool found = FALSE;
    
    argument = one_argument( argument , arg1);
    argument = one_argument( argument , arg2);
    argument = one_argument( argument , arg3);
    

    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                
                if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
                if (  (ship = ship_from_navseat(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be at a nav computer to calculate jumps.\n\r",ch);
    	            return;
    	        }

                if ( autofly(ship)  )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first....\n\r",ch);
    	            return;
    	        }
    	        
                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RAnd what exactly are you going to calculate...?\n\r" , ch );
                   return;
                }   
    	        if (ship->mod->hyperspeed == 0)
                {
                  send_to_char("&RThis ship is not equipped with a hyperdrive!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_LANDED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->spaceobject == NULL)
    	        {
    	            send_to_char("&RYou can only do that in realspace.\n\r",ch);
    	            return;
    	        }
    	        if (arg1[0] == '\0')
    	        {
    	            send_to_char("&WFormat: Calculate <spaceobject> <entry x> <entry y> <entry z>\n\r&wPossible destinations:\n\r",ch);
/*    	            for ( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
                    {
                       set_char_color( AT_NOTE, ch );
                       ch_printf(ch,"%-30s %d\n\r",spaceobj->name,
                                 (abs(spaceobj->xpos - ship->spaceobj->xpos)+
                                 abs(spaceobj->ypos - ship->spaceobj->ypos))/2);
                       count++;
                    }
                    if ( !count )
                    {
                        send_to_char( "No spacial objects found.\n\r", ch );
                    }
*/                  return;
    	        }
                chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_navigation]) ;
                if ( number_percent( ) > chance )
    		{
	           send_to_char("&RYou cant seem to figure the charts out today.\n\r",ch);
	           learn_from_failure( ch, gsn_navigation );
    	   	   return;
    	   	}

    if( arg2 && arg3 && arg2[0] != '\0' && arg3[0] != '\0')
    {
    	ship->jx = ship->vx + atoi(arg1);
    	ship->jy = ship->vy + atoi(arg2);
    	ship->jz = ship->vz + atoi(arg3);
    	found = TRUE;
    }
    else
    {
      send_to_char("&WFormat: Calculate x y z&R&w\n\r",ch);
      return;
    }    

        spaceobject = ship->currjump;
           
        if ( !found )
        {
          send_to_char( "&RYou can't seem to find that spacial object on your charts.\n\r", ch);
          ship->currjump = NULL;
          return;
        }

             ship->jx += number_range ( ((ship->mod->astro_array) - 300) , (300-(ship->mod->astro_array)) );
             ship->jy += number_range ( ((ship->mod->astro_array) - 300) , (300-(ship->mod->astro_array)) );
             ship->jz += number_range ( ((ship->mod->astro_array) - 300) , (300-(ship->mod->astro_array)) );
             ship->jx += (distance ? distance : (spaceobject && spaceobject->gravity ? spaceobject->gravity : 0 ) );
             ship->jy += (distance ? distance : (spaceobject && spaceobject->gravity ? spaceobject->gravity : 0 ) );
             ship->jz += (distance ? distance : (spaceobject && spaceobject->gravity ? spaceobject->gravity : 0 ) );

 	for ( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
          if ( !spaceobj->trainer && distance && strcmp(spaceobj->name,"") && 
                     abs( (int) ( ship->jx - spaceobj->xpos )) < (spaceobj->gravity*4) && 
                     abs( (int) ( ship->jy - spaceobj->ypos )) < (spaceobj->gravity*4) &&
                     abs( (int) ( ship->jz - spaceobj->zpos )) < (spaceobj->gravity*4) )
                {
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Jump coordinates too close to stellar object.");
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Hyperjump NOT set.");
                    ship->currjump = NULL;
                    return;
                }              

    for( spaceobject = first_spaceobject; spaceobject; spaceobject = spaceobject->next )
      if( space_in_range( ship, spaceobject ) )
      {
        ship->currjump = spaceobject;
        break;
      }
    if( !spaceobject )
      ship->currjump = ship->spaceobject;

    if( ship->jx > MAX_COORD_S || ship->jy > MAX_COORD_S || ship->jz > MAX_COORD_S ||
        ship->jx < -MAX_COORD_S || ship->jy < -MAX_COORD_S || ship->jz < -MAX_COORD_S )
    {
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Jump coordinates outside of the known galaxy.");
                    echo_to_cockpit( AT_RED, ship, "WARNING.. Hyperjump NOT set.");
                    ship->currjump = NULL;
                    return;
    }
    


    ship->hyperdistance  = abs( (int ) ( ship->vx - ship->jx )) ;
    ship->hyperdistance += abs( (int ) ( ship->vy - ship->jy )) ;
    ship->hyperdistance += abs( (int ) ( ship->vz - ship->jz )) ;
    ship->hyperdistance /= 200;

    if (ship->hyperdistance<100)
      ship->hyperdistance = 100;
      
    ship->orighyperdistance = ship->hyperdistance;

    sound_to_room( ch->in_room , "!!SOUND(computer)" );

    sprintf(buf, "&GHyperspace course set. Estimated distance: %d\n\rReady for the jump to lightspeed.\n\r", ship->hyperdistance );
    send_to_char( buf, ch);
    echo_to_docked( AT_YELLOW , ship, "The docking port link shows a new course being calculated." );

    act( AT_PLAIN, "$n does some calculations using the ships computer.", ch,
		        NULL, argument , TO_ROOM );
	                
    learn_from_success( ch, gsn_navigation );

    WAIT_STATE( ch , 2*PULSE_VIOLENCE );	
}


void do_recharge(CHAR_DATA *ch, char *argument )
{
    int recharge;
    int chance;
    SHIP_DATA *ship;
    
   
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        if (  (ship = ship_from_coseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RThe controls must be at the co-pilot station.\n\r",ch);
            return;
        }
        
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to power a recharge order.\n\r",ch);
    	            return;
    	        }
    	        
        if ( ship->energy < 100 )
        {
              send_to_char("&RTheres not enough energy!\n\r",ch);
              return;
        }
    	        
        chance = IS_NPC(ch) ? ch->top_level
                 : (int) (ch->pcdata->learned[gsn_shipsystems]);
        if ( number_percent( ) > chance )
        {
            send_to_char("&RYou fail to work the controls properly.\n\r",ch);
            learn_from_failure( ch, gsn_shipsystems );
    	   return;	
        }
        
    send_to_char( "&GRecharging shields..\n\r", ch);
    act( AT_PLAIN, "$n pulls back a lever on the control panel.", ch,
         NULL, argument , TO_ROOM );
    
    learn_from_success( ch, gsn_shipsystems );
     
    recharge  = 25+ship->shipclass*25;
    recharge  = UMIN(  ship->mod->maxshield-ship->shield , recharge );
    ship->shield += recharge;
    ship->energy -= ( recharge*2 + recharge * ship->shipclass );        
}


void do_repairship(CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance, change;
    SHIP_DATA *ship;
    TURRET_DATA *turret;
    strcpy( arg, argument );

    switch( ch->substate )
    {
    	default:
    	        if (  (ship = ship_from_engine(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the engine room of a ship to do that!\n\r",ch);
    	            return;
    	        }

                if ( str_cmp( argument , "hull" ) && str_cmp( argument , "drive" ) &&
                     str_cmp( argument , "launcher" ) && str_cmp( argument , "laser" ) &&
                     str_cmp( argument , "turret" ) &&
                     str_cmp( argument , "docking" ) && str_cmp( argument , "tractor" ) )
                {
                   send_to_char("&RYou need to spceify something to repair:\n\r",ch);
                   send_to_char("&rTry: hull, drive, launcher, laser, docking, tractor, or turret\n\r",ch);
                   return;
                }

                chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_shipmaintenance]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&GYou begin your repairs\n\r", ch);
    		   act( AT_PLAIN, "$n begins repairing the ships $T.", ch,
		        NULL, argument , TO_ROOM );
    		   if ( !str_cmp(arg,"hull") )
    		     add_timer ( ch , TIMER_DO_FUN , 15 , do_repairship , 1 );
    		   else
    		     add_timer ( ch , TIMER_DO_FUN , 5 , do_repairship , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou fail to locate the source of the problem.\n\r",ch);
	        learn_from_failure( ch, gsn_shipmaintenance );
    	   	return;

    	case 1:
    		if ( !ch->dest_buf )
    		   return;
    		strcpy(arg, (const char * ) ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		break;

    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		ch->substate = SUB_NONE;
    		if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    		      return;
    	        send_to_char("&RYou are distracted and fail to finish your repairs.\n\r", ch);
    		return;
    }

    ch->substate = SUB_NONE;
    
    if ( (ship = ship_from_engine(ch->in_room->vnum)) == NULL )
    {  
       return;
    }
    
    if ( !str_cmp(arg,"hull") )
    {
        change = URANGE( 0 , 
                         number_range( (IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_shipmaintenance]) / 2 ) , 
	                 (IS_NPC(ch) ? ch->top_level : (int) (ch->pcdata->learned[gsn_shipmaintenance]) ) ),
                         ( ship->mod->maxhull - ship->hull ) );
        ship->hull += change;
        ch_printf( ch, "&GRepair complete.. Hull strength inreased by %d points.\n\r", change );
    }
    
    if ( !str_cmp(arg,"drive") )
    {  
       if (ship->location == ship->lastdoc)
          ship->shipstate = SHIP_LANDED;
       else if ( ship->shipstate == SHIP_HYPERSPACE )
         send_to_char("You realize after working that it would be a bad idea to do this while in hyperspace.\n\r", ch);		
       else     
         ship->shipstate = SHIP_READY;
       send_to_char("&GShips drive repaired.\n\r", ch);		
    }

    if ( !str_cmp(arg,"docking") )
    {  
       ship->statetdocking = SHIP_READY;
       send_to_char("&GDocking bay repaired.\n\r", ch);
    }
    if ( !str_cmp(arg,"tractor") )
    {  
       ship->statettractor = SHIP_READY;
       send_to_char("&GTractorbeam repaired.\n\r", ch);
    }
    if ( !str_cmp(arg,"launcher") )
    {  
       ship->missilestate = MISSILE_READY;
       send_to_char("&GMissile launcher repaired.\n\r", ch);
    }
    
    if ( !str_cmp(arg,"laser") )
    {  
       ship->statet0 = LASER_READY;
       send_to_char("&GMain laser repaired.\n\r", ch);
    }

    if ( !str_cmp(arg,"turret") )
    {  
       if( ship->first_turret )
         for( turret = ship->first_turret; turret; turret = turret->next )
           if( ch->in_room->vnum == turret->roomvnum )
           {
	     turret->state = LASER_READY;
       	     send_to_char("&GLaser Turret repaired.\n\r", ch);
       	   }
    }
    
    act( AT_PLAIN, "$n finishes the repairs.", ch,
         NULL, argument , TO_ROOM );

    learn_from_success( ch, gsn_shipmaintenance );
    	
}

void do_addpilot(CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *ship;
    int chance;
     
   if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
   {
           send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
           return;
   }

   chance = number_percent( );
   
   if ( IS_NPC(ch) || chance >= ch->pcdata->learned[gsn_slicing] )
   { 
      if ( !check_pilot( ch , ship ) )
         {
           send_to_char( "&RThat isn't your ship!" ,ch );
   	   return;
         }
   }

   if (argument[0] == '\0')
   {
       send_to_char( "&RAdd which pilot?\n\r" ,ch );
       return;
   }
   
   if ( chance < ch->pcdata->learned[gsn_slicing] ) 
     learn_from_success( ch, gsn_slicing );

   if ( str_cmp( ship->pilot , "" ) )
   {
        if ( str_cmp( ship->copilot , "" ) )
        {
             send_to_char( "&RYou already have a pilot and copilot..\n\r" ,ch );
             send_to_char( "&RTry rempilot first.\n\r" ,ch );
             return;
        }
        
        STRFREE( ship->copilot );
	ship->copilot = STRALLOC( argument );
	send_to_char( "Copilot Added.\n\r", ch );
	save_ship( ship );
	return;
        
        return;
   }
   
   STRFREE( ship->pilot );
   ship->pilot = STRALLOC( argument );
   send_to_char( "Pilot Added.\n\r", ch );
   save_ship( ship );

}

void do_rempilot(CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    
   if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
   {
           send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
           return;
   }
   
                if  ( ship->shipclass == SHIP_PLATFORM )
                {
                   send_to_char( "&RYou can't do that here.\n\r" , ch );
                   return;
                }   
  chance = number_percent( );
  if ( IS_NPC(ch) || chance >= ch->pcdata->learned[gsn_slicing] )
  { 
       if ( !check_pilot( ch , ship ) )
         {
           send_to_char( "&RThat isn't your ship!" ,ch );
   	   return;
         }
  }

   if (argument[0] == '\0')
   {
       send_to_char( "&RRemove which pilot?\n\r" ,ch );
       return;
   }

   if ( chance < ch->pcdata->learned[gsn_slicing] ) 
     learn_from_success( ch, gsn_slicing );
     
   if ( !str_cmp( ship->pilot , argument ) )
   {
        STRFREE( ship->pilot );
	ship->pilot = STRALLOC( "" );
	send_to_char( "Pilot Removed.\n\r", ch );
	save_ship( ship );
	return;
   }       
   
   if ( !str_cmp( ship->copilot , argument ) )
   {      
        STRFREE( ship->copilot );
	ship->copilot = STRALLOC( "" );
	send_to_char( "Copilot Removed.\n\r", ch );
	save_ship( ship );
	return;
    }    

    send_to_char( "&RThat person isn't listed as one of the ships pilots.\n\r" ,ch );

}

void do_radar( CHAR_DATA *ch, char *argument )
{
    SHIP_DATA *target;
    int chance;
    SHIP_DATA *ship;
    MISSILE_DATA *missile;    
    SPACE_DATA *spaceobj;
        if (   (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit or turret of a ship to do that!\n\r",ch);
            return;
        }
        
        if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }

        if (ship->shipstate == SHIP_LANDED)
        {
          if (ship->docked == NULL) {
            send_to_char("&RWait until after you launch!\n\r",ch);
            return;
          }
        }
        
        if (ship->shipstate == SHIP_HYPERSPACE)
        {
            send_to_char("&RYou can only do that in realspace!\n\r",ch);
            return;
        }
        
    	if (ship->spaceobject == NULL)
    	{
    	       send_to_char("&RYou can't do that unless the ship is flying in realspace!\n\r",ch);
    	       return;
    	}        
    	        
        chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_navigation]) ;
        if ( number_percent( ) > chance )
        {
           send_to_char("&RYou fail to work the controls properly.\n\r",ch);
           learn_from_failure( ch, gsn_navigation );
    	   return;	
        }
        
    
    act( AT_PLAIN, "$n checks the radar.", ch,
         NULL, argument , TO_ROOM );
     
  	         set_char_color(  AT_RED, ch );
		 for ( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
		 {
    	           if ( space_in_range( ship, spaceobj ) && spaceobj->type == SPACE_SUN && str_cmp(spaceobj->name,"") )
    	                 ch_printf(ch, "%-15s%.0f %.0f %.0f\n\r%-15s%.0f %.0f %.0f\n\r" ,
    	                        spaceobj->name,
    	                        spaceobj->xpos,
    	                        spaceobj->ypos,
    	                        spaceobj->zpos, "",
    	                        (spaceobj->xpos - ship->vx),
    	                        (spaceobj->ypos - ship->vy),
    	                        (spaceobj->zpos - ship->vz) );
  		   }
  	         set_char_color(  AT_LBLUE, ch );
		 for ( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
		 {
    	           if ( space_in_range( ship, spaceobj ) && spaceobj->type == SPACE_PLANET && str_cmp(spaceobj->name,"") )
    	                 ch_printf(ch, "%-15s%.0f %.0f %.0f\n\r%-15s%.0f %.0f %.0f\n\r" ,
    	                        spaceobj->name,
    	                        spaceobj->xpos,
    	                        spaceobj->ypos,
    	                        spaceobj->zpos, "",
    	                        (spaceobj->xpos - ship->vx),
    	                        (spaceobj->ypos - ship->vy),
    	                        (spaceobj->zpos - ship->vz));
		 }    	           
		 ch_printf(ch,"\n\r");
  	         set_char_color(  AT_WHITE, ch );
		 for ( spaceobj = first_spaceobject; spaceobj; spaceobj = spaceobj->next )
		 {
    	           if ( space_in_range( ship, spaceobj ) && spaceobj->type > SPACE_PLANET && str_cmp(spaceobj->name,"") )
    	                 ch_printf(ch, "%-15s%.0f %.0f %.0f\n\r%-15s%.0f %.0f %.0f\n\r" ,
    	                        spaceobj->name,
    	                        spaceobj->xpos,
    	                        spaceobj->ypos, 
    	                        spaceobj->zpos, "",
    	                        (spaceobj->xpos - ship->vx),
    	                        (spaceobj->ypos - ship->vy),
    	                        (spaceobj->zpos - ship->vz) );
		 }    	           
		 ch_printf(ch,"\n\r");

   	         for ( target = first_ship; target; target = target->next )
                   {
                        if ( target != ship && target->spaceobject )
                        {
                          if ( abs( (int) ( target->vx - ship->vx )) < 100*(ship->mod->sensor+10)*((target->shipclass == SHIP_DEBRIS ? 2 : target->shipclass)+1) &&
                          abs( (int) ( target->vy - ship->vy )) < 100*(ship->mod->sensor+10)*((target->shipclass == SHIP_DEBRIS ? 2 : target->shipclass)+1) &&
                          abs( (int) ( target->vz - ship->vz )) < 100*(ship->mod->sensor+10)*((target->shipclass == SHIP_DEBRIS ? 2 : target->shipclass)+1) )

                            ch_printf(ch, "%s    %.0f %.0f %.0f\n\r",
                              target->name,
			      (target->vx - ship->vx),
			      (target->vy - ship->vy),
			      (target->vz - ship->vz));


                         else if ( abs( (int) ( target->vx - ship->vx )) < 100*(ship->mod->sensor+10)*((target->shipclass == SHIP_DEBRIS ? 2 : target->shipclass)+3) &&
                         abs( (int) ( target->vy - ship->vy )) < 100*(ship->mod->sensor+10)*((target->shipclass == SHIP_DEBRIS ? 2 : target->shipclass)+3) &&
                         abs( (int) ( target->vz - ship->vz )) < 100*(ship->mod->sensor+10)*((target->shipclass == SHIP_DEBRIS ? 2 : target->shipclass)+3) )
                         {
                           if ( target->shipclass == FIGHTER_SHIP )
                            ch_printf(ch, "A small metallic mass    %.0f %.0f %.0f\n\r",
			      (target->vx - ship->vx),
			      (target->vy - ship->vy),
			      (target->vz - ship->vz));
                           if ( target->shipclass == MIDSIZE_SHIP )
                            ch_printf(ch, "A goodsize metallic mass    %.0f %.0f %.0f\n\r",
			      (target->vx - ship->vx),
			      (target->vy - ship->vy),
			      (target->vz - ship->vz));
                           if ( target->shipclass == SHIP_DEBRIS )
                            ch_printf(ch, "scattered metallic reflections    %.0f %.0f %.0f\n\r",
			      (target->vx - ship->vx),
			      (target->vy - ship->vy),
			      (target->vz - ship->vz));
                           else if ( target->shipclass >= CAPITAL_SHIP )
                            ch_printf(ch, "A huge metallic mass    %.0f %.0f %.0f\n\r",
			      (target->vx - ship->vx),
			      (target->vy - ship->vy),
			      (target->vz - ship->vz));
                         }

                       }

                   }
                   ch_printf(ch,"\n\r");
    	           for ( missile = first_missile; missile; missile = missile->next )
                   {

                          if ( abs( (int) ( missile->mx - ship->vx )) < 50*(ship->mod->sensor+10)*2 &&
                          abs( (int) ( missile->my - ship->vy )) < 50*(ship->mod->sensor+10)*2 &&
                          abs( (int) ( missile->mz - ship->vz )) < 50*(ship->mod->sensor+10)*2 )
                          {
                           ch_printf(ch, "%s    %d %d %d\n\r",
                           	missile->missiletype == CONCUSSION_MISSILE ? "A Concusion missile" :
    			        ( missile->missiletype ==  PROTON_TORPEDO ? "A Torpedo" :
    			        ( missile->missiletype ==  HEAVY_ROCKET ? "A Heavy Rocket" : "A Heavy Bomb" ) ),
			      (missile->mx - ship->vx),
			      (missile->my - ship->vy),
			      (missile->mz - ship->vz));
                          }
                   }

                   ch_printf(ch, "\n\r&WYour Coordinates: %.0f %.0f %.0f\n\r" ,
                             ship->vx , ship->vy, ship->vz);
        
    	        
        learn_from_success( ch, gsn_navigation );
  
}

void do_autotrack( CHAR_DATA *ch, char *argument )
{
   SHIP_DATA *ship;
   int chance;
 
   if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
   {
        send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
        return;
   }
   
        if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
 
        if ( ship->shipclass == SHIP_PLATFORM )
    	        {
    	            send_to_char("&RPlatforms don't have autotracking systems!\n\r",ch);
    	            return;
    	        }
        if ( ship->shipclass == CAPITAL_SHIP )
    	        {
    	            send_to_char("&RThis ship is too big for autotracking!\n\r",ch);
    	            return;
    	        }
        if ( ship->docked != NULL )
              {
                 	send_to_char("&RYou can not autotrack while docked!\n\r",ch);
                  return;
              }

   if (  (ship = ship_from_pilotseat(ch->in_room->vnum))  == NULL )
   {
        send_to_char("&RYou aren't in the pilots chair!\n\r",ch);
        return;
   }
   
                if ( autofly(ship)  )
    	        {
    	            send_to_char("&RYou'll have to turn off the ships autopilot first....\n\r",ch);
    	            return;
    	        }
    	        
   chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_shipsystems]) ;
        if ( number_percent( ) > chance )
        {
           send_to_char("&RYour not sure which switch to flip.\n\r",ch);
           learn_from_failure( ch, gsn_shipsystems );
    	   return;	
        }
   
   act( AT_PLAIN, "$n flips a switch on the control panel.", ch,
         NULL, argument , TO_ROOM );
   if (ship->autotrack)
   {
     ship->autotrack = FALSE;
     echo_to_cockpit( AT_YELLOW , ship, "Autotracking off.");
   }
   else
   {
      ship->autotrack = TRUE;
      echo_to_cockpit( AT_YELLOW , ship, "Autotracking on.");
   }
   
   learn_from_success( ch, gsn_shipsystems );
        
}

void do_findserin( CHAR_DATA *ch, char *argument )
{
     bool ch_comlink = FALSE;
     OBJ_DATA *obj;
     int next_planet, bus, stopnum = 1;
     char arg[MAX_INPUT_LENGTH];
     char arg2[MAX_INPUT_LENGTH];
     argument = one_argument( argument, arg );
     strcpy ( arg2, argument);


     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
     {
         if (obj->pIndexData->item_type == ITEM_COMLINK)
         ch_comlink = TRUE;
     }

     if ( !ch_comlink && !IS_IMMORTAL(ch))
     {
        send_to_char( "You need a comlink to do that!\n\r", ch);
        return;
     }

     if ( !arg || arg[0] == '\0' )
     {
         ch_printf( ch, "&cList of Serins:  \n\n\r" );
	 for( bus = 0; bus < MAX_BUS; bus++ )
           ch_printf( ch, "&C%s &c- ", serin[bus].name );
  	 return;
     }
	

     for( bus = 0; bus < MAX_BUS; bus++ )
       if( !str_cmp( serin[bus].name, arg ) )
         break;
     if( bus == MAX_BUS )
     {
        send_to_char( "No such serin!\n\r", ch);
        return;
     }

     if ( bus_pos < 7 && bus_pos > 1 )
       ch_printf( ch, "The %s is Currently docked at %s.\n\r", serin[bus].name, serin[bus].bus_stop[serin[bus].planetloc] );

     next_planet = serin[bus].planetloc;
     send_to_char( "Next stops: ", ch);
     if ( bus_pos <= 1 )
        ch_printf( ch, "%s  ", serin[bus].bus_stop[next_planet] );

     while ( serin[bus].bus_vnum[stopnum] != serin[bus].bus_vnum[0] )
       stopnum++;
     stopnum--;
     for ( ;; )
     {
         next_planet++;
         if ( next_planet > stopnum )
            next_planet = 0;
         ch_printf( ch, "%s  ", serin[bus].bus_stop[next_planet] );
         if( serin[bus].planetloc == next_planet )
           break;
     }

     ch_printf( ch, "\n\r\n\r" );

}

void do_pluogus( CHAR_DATA *ch, char *argument )
{
  do_findserin( ch, "pluogus" );
  return;
}

#if 0
void do_pluogus( CHAR_DATA *ch, char *argument )
{
     bool ch_comlink = FALSE;
     OBJ_DATA *obj;
     int next_planet, itt;

     for ( obj = ch->last_carrying; obj; obj = obj->prev_content )
     {                               
         if (obj->pIndexData->item_type == ITEM_COMLINK)
         ch_comlink = TRUE;
     }

     if ( !ch_comlink )
     {
        send_to_char( "You need a comlink to do that!\n\r", ch);
        return;
     }         
     
     send_to_char( "Serin Pluogus Schedule Information:\n\r", ch );

     /* current port */

     if ( bus_pos < 7 && bus_pos > 1 )
        ch_printf( ch, "The Pluogus is Currently docked at %s.\n\r", bus_stop[bus_planet] );

     /* destinations */

     next_planet = bus_planet;
     send_to_char( "Next stops: ", ch);

     if ( bus_pos <= 1 )
        ch_printf( ch, "%s  ", bus_stop[next_planet] );

     for ( itt = 0 ; itt < 3 ; itt++ )
     {
         next_planet++;
         if ( next_planet >= MAX_BUS_STOP )
            next_planet = 0;
         ch_printf( ch, "%s  ", bus_stop[next_planet] );
     }

     ch_printf( ch, "\n\r\n\r" );

     send_to_char( "Serin Tocca Schedule Information:\n\r", ch );

     /* current port */

     if ( bus_pos < 7 && bus_pos > 1 )
        ch_printf( ch, "The Tocca is Currently docked at %s.\n\r", bus_stop[bus2_planet] );

     /* destinations */

     next_planet = bus2_planet;
     send_to_char( "Next stops: ", ch);

     if ( bus_pos <= 1 )
        ch_printf( ch, "%s  ", bus_stop[next_planet] );

     for ( itt = 0 ; itt < 3 ; itt++ )
     {
         next_planet++;
         if ( next_planet >= MAX_BUS_STOP )
            next_planet = 0;
         ch_printf( ch, "%s  ", bus_stop[next_planet] );
     }

     ch_printf( ch, "\n\r\n\r" );
     send_to_char( "Serin Cassia Schedule Information:\n\r", ch );

     /* current port */

     if ( bus_pos < 7 && bus_pos > 1 )
        ch_printf( ch, "The Cassia is Currently docked at %s.\n\r", bus_stoprev[bus3_planet] );

     /* destinations */

     next_planet = bus3_planet;
     send_to_char( "Next stops: ", ch);

     if ( bus_pos <= 1 )
        ch_printf( ch, "%s  ", bus_stoprev[next_planet] );

     for ( itt = 0 ; itt < 3 ; itt++ )
     {
         next_planet++;
         if ( next_planet >= MAX_BUS_STOP )
            next_planet = 0;
         ch_printf( ch, "%s  ", bus_stoprev[next_planet] );
     }

     ch_printf( ch, "\n\r\n\r" );

     send_to_char( "Serin Siego Schedule Information:\n\r", ch );

     /* current port */

     if ( bus_pos < 7 && bus_pos > 1 )
        ch_printf( ch, "The Siego is Currently docked at %s.\n\r", bus_stoprev[bus4_planet] );

     /* destinations */

     next_planet = bus4_planet;
     send_to_char( "Next stops: ", ch);

     if ( bus_pos <= 1 )
        ch_printf( ch, "%s  ", bus_stoprev[next_planet] );

     for ( itt = 0 ; itt < 3 ; itt++ )
     {
         next_planet++;
         if ( next_planet >= MAX_BUS_STOP )
            next_planet = 0;
         ch_printf( ch, "%s  ", bus_stoprev[next_planet] );
     }

     ch_printf( ch, "\n\r\n\r" );

}
#endif


void do_fly( CHAR_DATA *ch, char *argument )
{
        send_to_char( "Not implemented yet!\n\r", ch);
 }

void do_drive( CHAR_DATA *ch, char *argument )
{
    int dir;
    SHIP_DATA *ship, *target;
    char arg[MAX_INPUT_LENGTH];
    char arg2[MAX_INPUT_LENGTH];
    char buf[MAX_INPUT_LENGTH];
    
    argument = one_argument( argument, arg );
    strcpy ( arg2, argument);


        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the drivers seat of a land vehicle to do that!\n\r",ch);
            return;
        }
        
        if ( ship->shipclass < LAND_SPEEDER )
    	{
    	      send_to_char("&RThis isn't a land vehicle!\n\r",ch);
    	      return;
    	}
    	        
        
        if (ship->shipstate == SHIP_DISABLED)
    	{
    	     send_to_char("&RThe drive is disabled.\n\r",ch);
    	     return;
    	}
    	        
        if ( ship->energy <1 )
        {
              send_to_char("&RTheres not enough fuel!\n\r",ch);
              return;
        }
        
        if ( !strcmp( arg, "in" ))
        {
          target = ship_in_room( ship->in_room , arg2 );
            if ( !target )
            {
               act( AT_PLAIN, "I see no $T here.", ch, NULL, argument, TO_CHAR );
               return;
            }
            
          if (!target->hanger)
          {
            send_to_char("That ship does not have any room.\n\r", ch);
            return;
          }
          
          if (!target->bayopen)
          {
            send_to_char("The ship's bay doors must be open.\n\r", ch);
            return;
          }

          if ( IS_SET( target->in_room->room_flags, ROOM_INDOORS ) 
          || target->in_room->sector_type == SECT_INSIDE ) 
	  {
		send_to_char( "You can't drive indoors!\n\r", ch );
		return;
	  } 

          send_to_char("You drive the vehicle into the bay.\n\r", ch);           
          sprintf( buf, "%s drives into %s.", ship->name, target->name); 
          echo_to_room( AT_GREY,  ship->in_room, buf);
          
          transship(ship, target->hanger);
          
          sprintf( buf, "%s drives into the bay", ship->name);
          echo_to_room( AT_GREY, ship->in_room, buf);
          learn_from_success( ch, gsn_speeders );
          return;
      }
           
        if ( !strcmp( arg, "out" ))
        {
          target = ship_from_hanger(ship->in_room->vnum);
          if (!target)
          {
            send_to_char("You have to be in a ship's hanger to drive out of one.\n\r", ch);
            return;
          }
          
          if ( target->spaceobject != NULL )
          {
            send_to_char("The ship must be landed before you drive out of its hanger!\n\r", ch);
            return;
          }
          
          if (!target->bayopen)
          {
            send_to_char("The ship's bay doors must be open.\n\r", ch);
            return;
          }
          
          if ( IS_SET( target->in_room->room_flags, ROOM_INDOORS ) 
          || target->in_room->sector_type == SECT_INSIDE ) 
	  {
		send_to_char( "You can't drive indoors!\n\r", ch );
		return;
	  }
          
          send_to_char("You drive the vehicle out of the bay.\n\r", ch);           
          sprintf( buf, "%s drives out of the ship.", ship->name); 
          echo_to_room( AT_GREY,  ship->in_room, buf);
          
          transship(ship, target->in_room->vnum);
          
          sprintf( buf, "%s drives out of %s", ship->name, target->name);
          echo_to_room( AT_GREY, ship->in_room, buf);
          learn_from_success( ch, gsn_speeders );
          return;
      }
          
          

        if ( ( dir = get_door( arg ) ) == -1 )
        {
             send_to_char( "Usage: drive <direction>\n\r", ch );
             return;
        }
        
        drive_ship( ch, ship, get_exit(get_room_index(ship->location), dir), 0 );

}

ch_ret drive_ship( CHAR_DATA *ch, SHIP_DATA *ship, EXIT_DATA  *pexit , int fall )
{
    ROOM_INDEX_DATA *in_room;
    ROOM_INDEX_DATA *to_room;
    ROOM_INDEX_DATA *from_room;
    ROOM_INDEX_DATA *original;
    char buf[MAX_STRING_LENGTH];
    char *txt;
    char *dtxt;
    ch_ret retcode;
    sh_int door, distance, chance;
    bool drunk = FALSE;
    CHAR_DATA * rch;
    CHAR_DATA * next_rch;
    

    if ( !IS_NPC( ch ) )
      if ( IS_DRUNK( ch, 2 ) && ( ch->position != POS_SHOVE )
	&& ( ch->position != POS_DRAG ) )
	drunk = TRUE;

    if ( drunk && !fall )
    {
      door = number_door();
      pexit = get_exit( get_room_index(ship->location), door );
    }

#ifdef DEBUG
    if ( pexit )
    {
	sprintf( buf, "drive_ship: %s to door %d", ch->name, pexit->vdir );
	log_string( buf );
    }
#endif

    retcode = rNONE;
    txt = NULL;

    in_room = get_room_index(ship->location);
    from_room = in_room;
    if ( !pexit || (to_room = pexit->to_room) == NULL )
    {
	if ( drunk )
	  send_to_char( "You drive into a wall in your drunken state.\n\r", ch );
	 else
	  send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return rNONE;
    }

    door = pexit->vdir;
    distance = pexit->distance;

    if ( IS_SET( pexit->exit_info, EX_WINDOW )
    &&  !IS_SET( pexit->exit_info, EX_ISDOOR ) )
    {
	send_to_char( "Alas, you cannot go that way.\n\r", ch );
	return rNONE;
    }

    if (  IS_SET(pexit->exit_info, EX_PORTAL) 
       && IS_NPC(ch) )
    {
        act( AT_PLAIN, "Mobs can't use portals.", ch, NULL, NULL, TO_CHAR );
	return rNONE;
    }

    if ( IS_SET(pexit->exit_info, EX_NOMOB)
	&& IS_NPC(ch) )
    {
	act( AT_PLAIN, "Mobs can't enter there.", ch, NULL, NULL, TO_CHAR );
	return rNONE;
    }

    if ( IS_SET(pexit->exit_info, EX_CLOSED)
    && (IS_SET(pexit->exit_info, EX_NOPASSDOOR)) )
    {
	if ( !IS_SET( pexit->exit_info, EX_SECRET )
	&&   !IS_SET( pexit->exit_info, EX_DIG ) )
	{
	  if ( drunk )
	  {
	    act( AT_PLAIN, "$n drives into the $d in $s drunken state.", ch,
		NULL, pexit->keyword, TO_ROOM );
	    act( AT_PLAIN, "You drive into the $d in your drunken state.", ch,
		NULL, pexit->keyword, TO_CHAR ); 
	  }
	 else
	  act( AT_PLAIN, "The $d is closed.", ch, NULL, pexit->keyword, TO_CHAR );
	}
       else
	{
	  if ( drunk )
	    send_to_char( "You hit a wall in your drunken state.\n\r", ch );
	   else
	    send_to_char( "Alas, you cannot go that way.\n\r", ch );
	}

	return rNONE;
    }

/*
    if ( distance > 1 )
	if ( (to_room=generate_exit(in_room, &pexit)) == NULL )
	    send_to_char( "Alas, you cannot go that way.\n\r", ch );
*/
    if ( room_is_private( ch, to_room ) )
    {
	send_to_char( "That room is private right now.\n\r", ch );
	return rNONE;
    }

    if ( !IS_IMMORTAL(ch)
    &&  !IS_NPC(ch)
    &&  ch->in_room->area != to_room->area )
    {
	if ( ch->top_level < to_room->area->low_hard_range )
	{
	    set_char_color( AT_TELL, ch );
	    switch( to_room->area->low_hard_range - ch->top_level )
	    {
		case 1:
		  send_to_char( "A voice in your mind says, 'You are nearly ready to go that way...'", ch );
		  break;
		case 2:
		  send_to_char( "A voice in your mind says, 'Soon you shall be ready to travel down this path... soon.'", ch );
		  break;
		case 3:
		  send_to_char( "A voice in your mind says, 'You are not ready to go down that path... yet.'.\n\r", ch);
		  break;
		default:
		  send_to_char( "A voice in your mind says, 'You are not ready to go down that path.'.\n\r", ch);
	    }
	    return rNONE;
	}
	else
	if ( ch->top_level > to_room->area->hi_hard_range )
	{
	    set_char_color( AT_TELL, ch );
	    send_to_char( "A voice in your mind says, 'There is nothing more for you down that path.'", ch );
	    return rNONE;
	}          
    }

    if ( !fall )
    {
        if ( IS_SET( to_room->room_flags, ROOM_INDOORS ) 
        || IS_SET( to_room->room_flags, ROOM_SPACECRAFT )  
        || to_room->sector_type == SECT_INSIDE ) 
	{
		send_to_char( "You can't drive indoors!\n\r", ch );
		return rNONE;
	}
        
        if ( IS_SET( to_room->room_flags, ROOM_NO_DRIVING ) ) 
	{
		send_to_char( "You can't take a vehicle through there!\n\r", ch );
		return rNONE;
	}

	if ( in_room->sector_type == SECT_AIR
	||   to_room->sector_type == SECT_AIR
	||   IS_SET( pexit->exit_info, EX_FLY ) )
	{
            if ( ship->shipclass > CLOUD_CAR )
	    {
		send_to_char( "You'd need to fly to go there.\n\r", ch );
		return rNONE;
	    }
	}

	if ( in_room->sector_type == SECT_WATER_NOSWIM
	||   to_room->sector_type == SECT_WATER_NOSWIM 
	||   to_room->sector_type == SECT_WATER_SWIM
	||   to_room->sector_type == SECT_UNDERWATER
	||   to_room->sector_type == SECT_OCEANFLOOR )
	{

	    if ( ship->shipclass != OCEAN_SHIP )
	    {
		send_to_char( "You'd need a boat to go there.\n\r", ch );
		return rNONE;
	    }
	    	    
	}

	if ( IS_SET( pexit->exit_info, EX_CLIMB ) )
	{

	    if ( ship->shipclass < CLOUD_CAR )
	    {
		send_to_char( "You need to fly or climb to get up there.\n\r", ch );
		return rNONE;
	    }
	}

    }

    if ( to_room->tunnel > 0 )
    {
	CHAR_DATA *ctmp;
	int count = 0;
	
	for ( ctmp = to_room->first_person; ctmp; ctmp = ctmp->next_in_room )
	  if ( ++count >= to_room->tunnel )
	  {
		  send_to_char( "There is no room for you in there.\n\r", ch );
		return rNONE;
	  }
    }

      if ( fall )
        txt = "falls";
      else
      if ( !txt )
      {
	  if (  ship->shipclass < OCEAN_SHIP )
	      txt = "fly";
	  else
	  if ( ship->shipclass == OCEAN_SHIP  )
	  {
	      txt = "float";
	  }
	  else
	  if ( ship->shipclass > OCEAN_SHIP  )
	  {
	      txt = "drive";
	  }
      }

        chance = IS_NPC(ch) ? ch->top_level
          : (int)  (ch->pcdata->learned[gsn_speeders]) ;
        if ( number_percent( ) > chance )
        {
            send_to_char("&RYou can't figure out which switch it is.\n\r",ch);
            learn_from_failure( ch, gsn_speeders );
    	   return retcode;	
        }


      sprintf( buf, "$n %ss the vehicle $T.", txt );
      act( AT_ACTION, buf, ch, NULL, dir_name[door], TO_ROOM );
      sprintf( buf, "You %s the vehicle $T.", txt );
      act( AT_ACTION, buf, ch, NULL, dir_name[door], TO_CHAR );
      sprintf( buf, "%s %ss %s.", ship->name, txt, dir_name[door] );
      echo_to_room( AT_ACTION , get_room_index(ship->location) , buf );

      extract_ship( ship );
      ship_to_room(ship, to_room->vnum );
      
      ship->location = to_room->vnum;
      ship->lastdoc = ship->location;
    
      if ( fall )
        txt = "falls";
      else
	  if (  ship->shipclass < OCEAN_SHIP )
	      txt = "flys in";
	  else
	  if ( ship->shipclass == OCEAN_SHIP  )
	  {
	      txt = "floats in";
	  }
	  else
	  if ( ship->shipclass > OCEAN_SHIP  )
	  {
	      txt = "drives in";
	  }

      switch( door )
      {
      default: dtxt = "somewhere";	break;
      case 0:  dtxt = "the south";	break;
      case 1:  dtxt = "the west";	break;
      case 2:  dtxt = "the north";	break;
      case 3:  dtxt = "the east";	break;
      case 4:  dtxt = "below";		break;
      case 5:  dtxt = "above";		break;
      case 6:  dtxt = "the south-west";	break;
      case 7:  dtxt = "the south-east";	break;
      case 8:  dtxt = "the north-west";	break;
      case 9:  dtxt = "the north-east";	break;
      }

    sprintf( buf, "%s %s from %s.", ship->name, txt, dtxt );
    echo_to_room( AT_ACTION , get_room_index(ship->location) , buf );
    
    for ( rch = ch->in_room->last_person ; rch ; rch = next_rch )
    { 
        next_rch = rch->prev_in_room;
        original = rch->in_room;
        char_from_room( rch );
        char_to_room( rch, to_room );
        do_look( rch, "auto" );
        char_from_room( rch );
        char_to_room( rch, original );
    }
    
/*
    if (  CHECK FOR FALLING HERE
    &&   fall > 0 )
    {
	if (!IS_AFFECTED( ch, AFF_FLOATING )
	|| ( ch->mount && !IS_AFFECTED( ch->mount, AFF_FLOATING ) ) )
	{
	  set_char_color( AT_HURT, ch );
	  send_to_char( "OUCH! You hit the ground!\n\r", ch );
	  WAIT_STATE( ch, 20 );
	  retcode = damage( ch, ch, 50 * fall, TYPE_UNDEFINED );
	}
	else
	{
	  set_char_color( AT_MAGIC, ch );
	  send_to_char( "You lightly float down to the ground.\n\r", ch );
	}
    }

*/    
    learn_from_success( ch, gsn_speeders );
    return retcode;

}

void do_bomb( CHAR_DATA *ch, char *argument )
{        
        send_to_char( "Not implemented yet!\n\r", ch);
 }

void do_chaff( CHAR_DATA *ch, char *argument )
{
    int chance;
    SHIP_DATA *ship;
    
   
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
        if ( ship->shipclass > SHIP_PLATFORM )
    	        {
    	            send_to_char("&RThis isn't a spacecraft!\n\r",ch);
    	            return;
    	        }
    	        
        
        if (  (ship = ship_from_coseat(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RThe controls are at the copilots seat!\n\r",ch);
            return;
        }
        
                if ( autofly(ship) )
    	        {
    	            send_to_char("&RYou'll have to turn the autopilot off first...\n\r",ch);
    	            return;
    	        }
    	        
                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
    	        if (ship->shipstate == SHIP_LANDED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
                if (ship->chaff <= 0 )
    	        {
    	            send_to_char("&RYou don't have any chaff to release!\n\r",ch);
    	            return;
    	        }
                chance = IS_NPC(ch) ? ch->top_level
                 : (int)  (ch->pcdata->learned[gsn_weaponsystems]) ;
        if ( number_percent( ) > chance )
        {
            send_to_char("&RYou can't figure out which switch it is.\n\r",ch);
            learn_from_failure( ch, gsn_weaponsystems );
    	   return;	
        }
    
    ship->chaff--;
    
    ship->chaff_released++;
        
    send_to_char( "You flip the chaff release switch.\n\r", ch);
    act( AT_PLAIN, "$n flips a switch on the control pannel", ch,
         NULL, argument , TO_ROOM );
    echo_to_cockpit( AT_YELLOW , ship , "A burst of chaff is released from the ship.");
	  
    learn_from_success( ch, gsn_weaponsystems );

}


void modtrainer( SHIP_DATA *ship, sh_int shipclass )
{
/*
  switch(shipclass)
  {
    case 0:
      ship->maxenergy = 1000;
      ship->lasers = 0;
      ship->ions = 0;
      ship->shield = 0;
      ship->maxhull = 0;
      ship->realspeed = 1;
      ship->manuever = 0;
      ship->hyperspeed = 0;
      ship->maxmissiles = 0;
      ship->maxtorpedos = 0;
      ship->maxrockets = 0;
      ship->maxchaff = 0;
      break;
    case 1:
      ship->maxenergy = 5000;
      ship->lasers = 2;
      ship->ions = 2;
      ship->shield = 100;
      ship->maxhull = 1000;
      ship->realspeed = 120;
      ship->manuever = 120;
      ship->hyperspeed = 150;
      ship->maxmissiles = 5;
      ship->maxtorpedos = 2;
      ship->maxrockets = 1;
      ship->maxchaff = 1;
      break;
    case 2:
      ship->maxenergy = 10000;
      ship->lasers = 3;
      ship->ions = 3;
      ship->shield = 300;
      ship->maxhull = 2500;
      ship->realspeed = 90;
      ship->manuever = 60;
      ship->hyperspeed = 200;
      ship->maxmissiles = 8;
      ship->maxtorpedos = 3;
      ship->maxrockets = 2;
      ship->maxchaff = 8;
      break;
    case 3:
      ship->maxenergy = 20000;
      ship->lasers = 4;
      ship->ions = 4;
      ship->shield = 500;
      ship->maxhull = 10000;
      ship->realspeed = 30;
      ship->manuever = 0;
      ship->hyperspeed = 150;
      ship->maxmissiles = 25;
      ship->maxtorpedos = 10;
      ship->maxrockets = 5;
      ship->maxchaff = 50;
      break;
  }
*/  
  return;
}    
    
      

bool autofly( SHIP_DATA *ship )
{
 
     if (!ship)
        return FALSE;
     
     if ( ship->type == MOB_SHIP )
        return TRUE;
     
     if ( ship->autopilot )
        return TRUE;
     
     return FALSE;   

}

void makedebris( SHIP_DATA *ship )
{
  SHIP_DATA *debris;
  char buf[MAX_STRING_LENGTH];

  if ( ship->shipclass == SHIP_DEBRIS )
    return;
 
  CREATE( debris, SHIP_DATA, 1 );

  LINK( debris, first_ship, last_ship, next, prev );

  debris->owner 	= STRALLOC( "" );
  debris->copilot       = STRALLOC( "" );
  debris->pilot         = STRALLOC( "" );
  debris->home          = STRALLOC( "" );
  debris->type		= SHIP_CIVILIAN; 
 if( ship->type != MOB_SHIP )
  debris->type          = ship->type;
  debris->shipclass         = SHIP_DEBRIS;
  debris->lasers        = ship->mod->lasers  ;
  debris->missiles   = ship->missiles  ;
  debris->rockets        = ship->rockets  ;
  debris->torpedos        = ship->torpedos  ;
  debris->maxshield        = ship->mod->maxshield  ;
  debris->maxhull        = ship->mod->maxhull  ;
  debris->maxenergy        = ship->mod->maxenergy  ;
  debris->hyperspeed        = ship->hyperspeed  ;
  debris->chaff        = ship->chaff  ;
  debris->realspeed        = ship->mod->realspeed  ;
  debris->currspeed        = ship->currspeed  ;
  debris->manuever        = ship->mod->manuever  ;

  debris->spaceobject = NULL;
  debris->energy = 0;
  debris->hull = ship->mod->maxhull;
  debris->in_room=NULL;
  debris->next_in_room=NULL;
  debris->prev_in_room=NULL;
  debris->currjump=NULL;
  debris->target0=NULL;
  debris->autopilot = FALSE;
 
  update_ship_modules( debris );
 
  add_random_modules( debris, ship );
 
  strcpy( buf, "Debris of a " );
  strcat( buf, ship->name );
  debris->name		= STRALLOC( "Debris" );
  debris->personalname		= STRALLOC( "Debris" );
  debris->description	= STRALLOC( buf );
  
  ship_to_spaceobject( debris, ship->spaceobject );
  debris->vx = ship->vx;
  debris->vy = ship->vy;
  debris->vz = ship->vz;
  debris->hx = ship->hx;
  debris->hy = ship->hy;
  debris->hz = ship->hz;
  
  return;
  
}

void shipdelete(SHIP_DATA * ship, bool shiplist) {

  char buf[MAX_STRING_LENGTH];
  char buf2[MAX_STRING_LENGTH];
  
  sprintf( buf, "%s%s", SHIP_DIR, ship->filename );
  sprintf( buf2, "%s%s", BACKUPSHIP_DIR, ship->filename );
  
  rename( buf, buf2 );

  UNLINK (ship, first_ship, last_ship, next, prev);
  extract_ship(ship);
  STRFREE(ship->owner);
  STRFREE(ship->copilot);
  STRFREE(ship->pilot);
  STRFREE(ship->home);

  STRFREE(ship->name);
  STRFREE(ship->personalname);
  STRFREE(ship->description);
  STRFREE(ship->templatestring);

  if(ship->cockpitroom) {
    delete_room(ship->cockpitroom);
    ship->cockpitroom = NULL;
  }
  
  DISPOSE( ship->mod );
  DISPOSE( ship );
 
  if( shiplist )
    write_ship_list( );
 
  return; 
}

void storeship( SHIP_DATA *ship )
{
  transship( ship, RENTALSTORAGEROOM );
  
  return;
}

void dumpship( SHIP_DATA *ship, int destination )
{
  CHAR_DATA *ch;
  OBJ_DATA *obj;
  ROOM_INDEX_DATA *room;
  ROOM_INDEX_DATA *toroom;

  if( !( room = ship->cockpitroom ) )
    return;

  if( !( toroom = get_room_index( destination ) ) )
    return;
    
      ch = room->first_person;
      while (ch) {
        char_from_room (ch);
        char_to_room (ch, toroom);
  send_to_char( "You exit the remains of your escape pod and enter your salvager's ship.", ch );
  ch = room->first_person;
      }

  obj = room->first_content;
  while ( obj ) {
    obj_from_room( obj );
    obj_to_room( obj, toroom );
    obj = room->first_content;
  }

//storeship( ship );
  return; 
}

SHIP_DATA *create_virtual_ship( SHIP_DATA *shiptemplate )
{
  SHIP_DATA *ship;
  SHIP_MOD_DATA *ship_mod;
  ROOM_INDEX_DATA *cockpitroom;
  ROOM_INDEX_DATA *templateroom;
  char buf[MAX_STRING_LENGTH];
  int roomvnum;

  if ( !shiptemplate )
    return NULL;

  CREATE (ship, SHIP_DATA, 1);
  
  roomvnum    = 100000 + (currrentalvnum ^ 65535);
  currrentalvnum++;
  templateroom = get_room_index(shiptemplate->cockpit);
 
  LINK (ship, first_ship, last_ship, next, prev);

  ship->owner         = STRALLOC ("");
  ship->copilot       = STRALLOC ("");
  ship->pilot         = STRALLOC ("");
  ship->home          = STRALLOC ("");
  ship->type          = SHIP_CIVILIAN;
  ship->shipclass         = shiptemplate->shipclass;
  ship->lasers        = shiptemplate->lasers;
  ship->missiles      = shiptemplate->missiles;
  ship->rockets       = shiptemplate->rockets;
  ship->torpedos      = shiptemplate->torpedos;
  ship->maxshield     = shiptemplate->maxshield;
  ship->maxhull       = shiptemplate->maxhull;
  ship->maxenergy     = shiptemplate->maxenergy;
  ship->hyperspeed    = shiptemplate->hyperspeed;
  ship->chaff         = shiptemplate->chaff;
  ship->realspeed     = shiptemplate->realspeed;
  ship->currspeed     = shiptemplate->currspeed;
  ship->manuever      = shiptemplate->manuever;

  ship->maxextmodules = shiptemplate->maxextmodules;
  ship->maxintmodules = shiptemplate->maxintmodules;

  ship->entrance      = roomvnum;
  ship->engineroom    = roomvnum;
  ship->firstroom     = roomvnum;
  ship->lastroom      = roomvnum;
  ship->navseat       = roomvnum;
  ship->pilotseat     = roomvnum;
  ship->coseat        = roomvnum;
  ship->gunseat       = roomvnum;
  
  ship->shipstate     = SHIP_LANDED;
  ship->docking       = SHIP_READY;
  ship->statei0       = LASER_READY;
  ship->statet0       = LASER_READY;
  ship->statettractor = SHIP_READY;
  ship->statetdocking = SHIP_READY;
  ship->missilestate  = MISSILE_READY;

  ship->spaceobject   = NULL;
  ship->energy        = shiptemplate->energy;
  ship->hull          = shiptemplate->hull;
  ship->in_room       = get_room_index(45);
  ship->next_in_room  = NULL;
  ship->prev_in_room  = NULL;
  ship->currjump      = NULL;
  ship->target0       = NULL;
  ship->tractoredby   = NULL;
  ship->tractored     = NULL;
  ship->docked        = NULL;
  ship->autopilot     = FALSE;
  
  CREATE( ship_mod, SHIP_MOD_DATA, 1 );
  ship->mod = ship_mod;
  update_ship_modules(ship);
  
  ship->name          = STRALLOC (shiptemplate->name);
  sprintf( buf, "%d",roomvnum );
  ship->personalname  = STRALLOC (buf);
  ship->description   = STRALLOC (shiptemplate->description);

  cockpitroom = make_room( roomvnum );

  cockpitroom->area    = templateroom->area;
  cockpitroom->sector_type = templateroom->sector_type;
  cockpitroom->room_flags  = templateroom->room_flags;
  cockpitroom->name = STRALLOC( templateroom->name );
  cockpitroom->description = STRALLOC( templateroom->description );
  
  ship->cockpitroom = cockpitroom;
  
  transship( ship, 45 );

  return ship;
}

void create_rental( CHAR_DATA *ch, SHIP_DATA *shiptemplate )
{
  SHIP_DATA *ship;
  ROOM_INDEX_DATA *room;
  
  if ( ch->gold < ( get_ship_value( shiptemplate )/100 ) )
  {
    ch_printf( ch, "You do not currently have the %d needed to rent this ship./n/r", get_ship_value( shiptemplate ) / 100 );
    return;
  }

  ch_printf( ch, "You rent the %s for %d credits.\n\r", shiptemplate->name, (get_ship_value(shiptemplate)/100) );

  if( (room = get_room_index(RENTALSTORAGEROOM)) != NULL )
  {
    for( ship = first_ship; ship; ship = ship->next )
      if( ship->location == room->vnum && !str_cmp( ship->name, shiptemplate->name ) )
        break;
  }
  else
  {
    ch_printf( ch, "Rental storage room not found.  Contact DV!/n/r" );
    return;
  }

  if ( !ship && !(ship = create_virtual_ship( shiptemplate )) )
    send_to_char( "ERROR! Please contact your administrator.\n\r", ch );

  transship( ship, ch->in_room->vnum );

  STRFREE(ship->owner);
  ship->owner = STRALLOC( "Public" );
  
  ch->gold -= get_ship_value( shiptemplate ) / 100;
  
  return;
}

void do_rent( CHAR_DATA *ch, char *argument )
{
  SHIP_DATA *ship;
  sh_int rentnum;
  
  if ( !argument || argument[0] == '\0' )
    for( rentnum = 0; rentnum < MAX_RENTALS; rentnum++ )
    {
      ship = ship_from_cockpit( rentals[rentnum].templatevnum );
      if ( !ship ) 
        ch_printf( ch, "[%d] ERROR-Non-existent ship\n\r", rentnum );
      else
        ch_printf( ch, "[%d] %.50s %d to rent.\n\r", rentnum, ship->name, (get_ship_value(ship)/100) );
      return;
    }

  if ( !IS_SET( ch->in_room->room_flags, ROOM_CAN_LAND ) )
  {
    send_to_char( "You can not rent ships here\n\r", ch );
    return;
  }

  for ( rentnum = 0; rentnum < MAX_RENTALS; rentnum++ )
  {
    ship = ship_from_cockpit( rentals[rentnum]. templatevnum );
    if ( ship && !str_cmp( argument, ship->name ) )
    {
      create_rental(ch, ship);
      return;
    }
  }
  for ( rentnum = 0; rentnum < MAX_RENTALS; rentnum++ )
  {
    ship = ship_from_cockpit( rentals[rentnum]. templatevnum );
    if ( ship && ship->name && (ship->name[0] != '\0') && nifty_is_name_prefix( argument, ship->name ) )
    {
      create_rental(ch, ship);
      return;
    }
  }
  ch_printf( ch, "There is no such rental choice. (%s)", argument );
  return;
}


/* Generic Pilot Command To use as template

void do_hmm( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    
    strcpy( arg, argument );    
    
    switch( ch->substate )
    { 
    	default:
    	        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
    	        {
    	            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
    	            return;
    	        }
                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate == SHIP_LANDED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current manouver.\n\r",ch);
    	            return;
    	        }
        
                if ( ship->energy <1 )
    	        {
    	           send_to_char("&RTheres not enough fuel!\n\r",ch);
    	           return;
    	        }

                if ( ship->shipclass == FIGHTER_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
                if ( ship->shipclass == MIDSIZE_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
                if ( ship->shipclass == CAPITAL_SHIP )
                    chance = IS_NPC(ch) ? ch->top_level
	                 : (int) (ch->pcdata->learned[gsn_capitalships]);
                if ( number_percent( ) < chance )
    		{
    		   send_to_char( "&G\n\r", ch);
    		   act( AT_PLAIN, "$n does  ...", ch,
		        NULL, argument , TO_ROOM );
		   echo_to_room( AT_YELLOW , get_room_index(ship->cockpit) , "");
    		   add_timer ( ch , TIMER_DO_FUN , 1 , do_hmm , 1 );
    		   ch->dest_buf = str_dup(arg);
    		   return;
	        }
	        send_to_char("&RYou fail to work the controls properly.\n\r",ch);
	        if ( ship->shipclass == FIGHTER_SHIP )
                    learn_from_failure( ch, gsn_starfighters );
                if ( ship->shipclass == MIDSIZE_SHIP )
    	            learn_from_failure( ch, gsn_midships );
                if ( ship->shipclass == CAPITAL_SHIP )
                    learn_from_failure( ch, gsn_capitalships );
    	   	return;	
    	
    	case 1:
    		if ( !ch->dest_buf )
    		   return;
    		strcpy(arg, ch->dest_buf);
    		DISPOSE( ch->dest_buf);
    		break;
    		
    	case SUB_TIMER_DO_ABORT:
    		DISPOSE( ch->dest_buf );
    		ch->substate = SUB_NONE;
    		if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    		      return;    		                                   
    	        send_to_char("&Raborted.\n\r", ch);
    	        echo_to_room( AT_YELLOW , get_room_index(ship->cockpit) , "");
    		if (ship->shipstate != SHIP_DISABLED)
    		   ship->shipstate = SHIP_READY;
    		return;
    }
    
    ch->substate = SUB_NONE;
    
    if ( (ship = ship_from_cockpit(ch->in_room->vnum)) == NULL )
    {  
       return;
    }

    send_to_char( "&G\n\r", ch);
    act( AT_PLAIN, "$n does  ...", ch,
         NULL, argument , TO_ROOM );
    echo_to_room( AT_YELLOW , get_room_index(ship->cockpit) , "");

         
    if ( ship->shipclass == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->shipclass == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->shipclass == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
    	
}

void do_hmm( CHAR_DATA *ch, char *argument )
{
    char arg[MAX_INPUT_LENGTH];
    int chance;
    SHIP_DATA *ship;
    
   
        if (  (ship = ship_from_cockpit(ch->in_room->vnum))  == NULL )
        {
            send_to_char("&RYou must be in the cockpit of a ship to do that!\n\r",ch);
            return;
        }
        
                if (ship->shipstate == SHIP_HYPERSPACE)
                {
                  send_to_char("&RYou can only do that in realspace!\n\r",ch);
                  return;   
                }
                if (ship->shipstate == SHIP_DISABLED)
    	        {
    	            send_to_char("&RThe ships drive is disabled. Unable to manuever.\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate == SHIP_LANDED)
    	        {
    	            send_to_char("&RYou can't do that until after you've launched!\n\r",ch);
    	            return;
    	        }
    	        if (ship->shipstate != SHIP_READY)
    	        {
    	            send_to_char("&RPlease wait until the ship has finished its current manouver.\n\r",ch);
    	            return;
    	        } 
        
        if ( ship->energy <1 )
        {
              send_to_char("&RTheres not enough fuel!\n\r",ch);
              return;
        }
    	        
        if ( ship->shipclass == FIGHTER_SHIP )
             chance = IS_NPC(ch) ? ch->top_level
             : (int)  (ch->pcdata->learned[gsn_starfighters]) ;
        if ( ship->shipclass == MIDSIZE_SHIP )
             chance = IS_NPC(ch) ? ch->top_level
                 : (int)  (ch->pcdata->learned[gsn_midships]) ;
        if ( ship->shipclass == CAPITAL_SHIP )
              chance = IS_NPC(ch) ? ch->top_level
                 : (int) (ch->pcdata->learned[gsn_capitalships]);
        if ( number_percent( ) > chance )
        {
            send_to_char("&RYou fail to work the controls properly.\n\r",ch);
            if ( ship->shipclass == FIGHTER_SHIP )
               learn_from_failure( ch, gsn_starfighters );
            if ( ship->shipclass == MIDSIZE_SHIP )   
               learn_from_failure( ch, gsn_midships );
            if ( ship->shipclass == CAPITAL_SHIP )
                learn_from_failure( ch, gsn_capitalships );
    	   return;	
        }
        
    send_to_char( "&G\n\r", ch);
    act( AT_PLAIN, "$n does  ...", ch,
         NULL, argument , TO_ROOM );
    echo_to_room( AT_YELLOW , get_room_index(ship->cockpit) , "");
	  
    
    
    if ( ship->shipclass == FIGHTER_SHIP )
        learn_from_success( ch, gsn_starfighters );
    if ( ship->shipclass == MIDSIZE_SHIP )
        learn_from_success( ch, gsn_midships );
    if ( ship->shipclass == CAPITAL_SHIP )
        learn_from_success( ch, gsn_capitalships );
}
*/

