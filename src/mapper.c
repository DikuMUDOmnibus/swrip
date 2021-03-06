#include <sys/types.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "mud.h"

/* Size of the map and depth of recursion to undertake */
#define MAPX     10
#define MAPY      8
#define MAXDEPTH  4

/* Ensure coord is on the map */
#define BOUNDARY(x, y) (((x) < 0) || ((y) < 0) || \
                        ((x) > MAPX) || ((y) > MAPY))

/* Structure for the map itself */
struct map_type
{
    /* Character to print at this map coord */
    char mapch;
    /* Room this coord represents */
    int vnum;
    /* Recursive depth this coord was found at */
    int depth;
    int info;
    bool can_see;
};

/* Chars for each of the four compass direction exits */
char map_chars[5] = "|-|-";
/* The map itself */
struct map_type map[MAPX + 1][MAPY + 1];

/* Take care of some repetitive code for later */
void get_exit_dir( int dir, int *x, int *y, int xorig, int yorig )
{
    /* Get the next coord based on direction */
    switch( dir )
    {
    /* North */
    case 0:
        *x = xorig;
        *y = yorig - 1;
        break;
    /* East */
    case 1:
        *x = xorig + 1;
        *y = yorig;
        break;
    /* South */
    case 2:
        *x = xorig;
        *y = yorig + 1;
        break;
    /* West */
    case 3:
        *x = xorig - 1;
        *y = yorig;
        break;
    case 6:
        *x = xorig + 1;
        *y = yorig - 1;
        break;
    case 7:
        *x = xorig - 1;
        *y = yorig - 1;
        break;
    case 8:
        *x = xorig + 1;
        *y = yorig + 1;
        break;
    case 9:
        *x = xorig - 1;
        *y = yorig + 1;
        break;
        
    default:
        *x = -1;
        *y = -1;
        break;
    }
}

/* Clear one map coord */
void clear_coord( int x, int y )
{
    map[x][y].mapch = ' ';
    map[x][y].vnum = 0;
    map[x][y].depth = 0;
    map[x][y].info = 0;
    map[x][y].can_see = TRUE;
}

/* Clear all exits for one room */
void clear_room( int x, int y )
{
    int dir, exitx, exity;

    /* Cycle through the four directions */
    for( dir = 0; dir < 10; dir++ )
    {
        /* Find next coord in this direction */
        get_exit_dir( dir, &exitx, &exity, x, y );
        /* If coord is valid, clear it */
        if ( !BOUNDARY( exitx, exity ) )
            clear_coord( exitx, exity );
    }
}

/* This function is recursive, ie it calls itself */
void map_exits( CHAR_DATA *ch, ROOM_INDEX_DATA *pRoom,
                int x, int y, int depth )
{
    int door;
    int exitx = 0, exity = 0;
    int roomx = 0, roomy = 0;
    EXIT_DATA *pExit;
/*
    char *buf;
    
    for ( rch = pRoom->first_person; rch; rch = rch->next_in_room )
	count++;
   
    sprintf( buf, "%d", count );
*/
    /* Setup this coord as a room */
    map[x][y].mapch = 'O';
    map[x][y].vnum = pRoom->vnum;
    map[x][y].depth = depth;
    map[x][y].info = pRoom->room_flags;
    map[x][y].can_see = room_is_dark( pRoom );

    /* Limit recursion */
    if ( depth > MAXDEPTH )
        return;

    /* This room is done, deal with it's exits */
    for( door = 0; door < 10; door++ )
    {
        /* Skip if there is no exit in this direction */
/*      for (xit = pRoom->first_exit; xit; xit = xit->next )
        if ( xit->vdir == door )
          break;
      if ( !xit )
        continue;
*/
      if ( ( pExit = get_exit( pRoom, door ) )== NULL )
           continue;

        /* Get the coords for the next exit and room in this direction */
        get_exit_dir( door, &exitx, &exity, x, y );
        get_exit_dir( door, &roomx, &roomy, exitx, exity );

        /* Skip if coords fall outside map */
        if ( BOUNDARY( exitx, exity ) || BOUNDARY( roomx, roomy ) )
            continue;

        /* Skip if there is no room beyond this exit */
        if ( pExit->to_room == NULL )
            continue;

        /* Ensure there are no clashes with previously defined rooms */
        if ( ( map[roomx][roomy].vnum != 0 ) &&
             ( map[roomx][roomy].vnum != pExit->to_room->vnum ) )
        {
            /* Use the new room if the depth is higher */
            if ( map[roomx][roomy].depth <= depth )
                continue;

            /* It is so clear the old room */
            clear_room( roomx, roomy );
        }

        /* No exits at MAXDEPTH */
        if ( depth == MAXDEPTH )
            continue;

        /* No need for exits that are already mapped */
        if ( map[exitx][exity].depth > 0 )
            continue;

        /* Fill in exit */
        map[exitx][exity].depth = depth;
        map[exitx][exity].vnum = pExit->to_room->vnum;
        map[exitx][exity].info = pExit->exit_info;
//      sprintf( buf, "%c", map_chars[door] );
        map[exitx][exity].mapch = map_chars[door];

        /* Place Marker 1 - referred to later */

        /* More to do? If so we recurse */
        if ( ( depth < MAXDEPTH ) &&
             ( ( map[roomx][roomy].vnum == pExit->to_room->vnum ) ||
               ( map[roomx][roomy].vnum == 0 ) ) )
        {
            /* Depth increases by one each time */
            map_exits( ch, pExit->to_room, roomx, roomy, depth + 1 );
        }
    }
}

/* Reformat room descriptions to exclude undesirable characters */
#if 0
void reformat_desc( char *desc )
{
    /* Index variables to keep track of array/pointer elements */
    int i, j;
    char buf[MAX_STRING_LENGTH], *p;

    i = 0;
    j = 0;
    buf[0] = '\0';

    if ( IS_NULLSTR( desc ) )
        return;

    /* Replace all "\n" and "\r" with spaces */
    for( i = 0; i <= strlen( desc ); i++ )
    {
        if ( ( desc[i] == '\n' ) || ( desc[i] == '\r' ) )
            desc[m] = ' ';
    }

    /* Remove multiple spaces */
    for( p = desc; *p != '\0'; p++ )
    {
        buf[j] = *p;
        j++;

        /* Two or more consecutive spaces? */
        if ( ( *p == ' ' ) && ( *( p + 1 ) == ' ' ) )
        {
            do
            {
                p++;
            } while( *p == ' ' );
        }
    }

    buf[j] = '\0';

    /* Copy to desc */
    sprintf( desc, buf );
}

int get_line( char *desc, int max_len )
{
    int i, j;

    /* Return if it's short enough for one line */
    if ( strlen( desc ) <= max_len )
        return 0;

    /* Calculate end point in string without color */
    for( i = 0; i <= strlen( desc ); i++ )
    {
        /* Here you need to skip your color sequences */
      if( (desc[j] == '&' && desc[j+1] != '&') || (desc[j] == '^' && desc[j+1] != '^')
        j--;
      else
        j++;

        if ( j > max_len )
            break;
    }

    /* End point is now in i, find the nearest space */
    for( j = i; j > 0; j-- )
    {
        if ( desc[j] == ' ' )
            break;
    }

    /* There could be a problem if there are no spaces on the line */

    return j + 1;
}
#endif
/* Display the map to the player */
void show_map( CHAR_DATA *ch, char *text )
{
    char buf[MAX_STRING_LENGTH * 2];
    int x, y, pos;
    char *p;

    pos = 0;
    p = text;
    buf[0] = '\0';

    /* Place Marker 2 - referred to later */

    /* Top of map frame */
    sprintf( buf, "+-----------+ " );
    /* First line of text */
    strcat( buf, "\n\r" );

    /* Write out the main map area with text */
    for( y = 0; y <= MAPY; y++ )
    {
        strcat( buf, "|" );

        for( x = 0; x <= MAPX; x++ )
        {
            /* Choose a color based on map contents here */

            /* Write the map character */
            sprintf( buf + strlen( buf ), "%c", map[x][y].mapch );
        }

        strcat( buf, "| \n\r" );
        /* Add the text, if necessary */
    }

    /* Finish off map area */
    strcat( buf, "+-----------+ " );

    /* Deal with any leftover text */

    /* Act can also be used here, as can send_to_char if desired */
    send_to_pager( buf, ch );
}

/* Clear, generate and display the map */
void do_draw( CHAR_DATA *ch, char *desc )
{
    int x, y;
    static char buf[MAX_STRING_LENGTH];
    OBJ_DATA *device;
    //sprintf( buf, desc );
    /* Remove undesirable characters */
    //reformat_desc( buf );

    if ( ( device = get_eq_char( ch, WEAR_HOLD ) ) == NULL )
    {
	send_to_char( "You must have a scanner to draw a map of the surrounding area.\n\r", ch );
	return;
    }

    if ( device->item_type != ITEM_DEVICE )
    {
	send_to_char( "You must have a scanner to draw a map of the surrounding area.\n\r", ch );
	return;
    }

    if (device->value[3] != 52 )
    {
	send_to_char( "You must have a scanner to draw a map of the surrounding area.\n\r", ch );
	return;
    }

    if ( device->value[2] <= 0 )
    {
        send_to_char( "Your scanner has no more charge left.", ch);
        return;
    }

    
    device->value[2]--;

    /* Clear map */
    for( y = 0; y <= MAPY; y++ )
    {
        for( x = 0; x <= MAPX; x++ )
        {
            clear_coord( x, y );
        }
    }

    /* Start with players pos at centre of map */
    x = MAPX / 2;
    y = MAPY / 2;

    map[x][y].vnum = ch->in_room->vnum;
    map[x][y].depth = 0;

    /* Generate the map */
    map_exits( ch, ch->in_room, x, y, 0 );

    /* Current position should be a "X" */
//  sprintf( buf2, "%c", 'X' );
    map[x][y].mapch = 'X';
    /* Send the map */
    show_map( ch, buf );
}
