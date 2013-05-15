/**

    @file       main.cpp
    @brief      Simple HTML Example

    The Simple HTML Example is a basic demonstration of the NetBurner web
    server implementation. In this example, an HTML page is protected by
    plain text authentication.

    This is a very basic authentication example, and only tests that the
    username and password are identical. It's also worth noting
    that the web server will cache the username/password that is initially
    entered, so to retry the example, you would need to clear your cache.

**/

#include "predef.h"
#include <constants.h>
#include <stdio.h>
#include <ctype.h>
#include <ucos.h>
#include <init.h>
#include <system.h>
#include <httppass.h>          // Needed for HTML passwords
#include <string.h>
#include <http.h>

const char *AppName = "HTML Simple Password Example";

// To use passwords we need to provide our own get handler.
static http_gethandler *oldhand;

/*-------------------------------------------------------------------
 * Function to test passwords, return 0 for failure. You can
 * modify this function to match whatever authentication you wish
 * to have.
 * ----------------------------------------------------------------*/
int TestPassword( const char *pPass, const char *pUser )
{
    // We will pass any user name and password that are not equal
    iprintf( "Testing PAssword = %s User = %s \r\n", pPass, pUser );
    return ( strcmp( pPass, pUser ) );
}

/*-------------------------------------------------------------------
 * By creating a HTTP GET handler, and registering it with
 * SetNewGetHandler( MyDoGet ), this function will be called in
 * response to a HTTP GET request instead of the system GET handler.
 * ----------------------------------------------------------------*/
int MyDoGet( int sock, PSTR url, PSTR rxBuffer )
{
    // Test for our password protected HTML page name
    if ( ( url[0] != 0 ) && ( httpstricmp( url, "P.HTM" ) == 1 ) )
    {
        // We need to do the password
        char *pPass;
        char *pUser;

        /* Check the HTTP request for password information. This
         * function examines the PASSWORD field stored in the HTTP
         * request.
         */
        if ( !CheckAuthentication( url, &pPass, &pUser ) ) {
            // Reject the current HTTP request, and request Password again.
            RequestAuthentication( sock, "FillInThisName" );
            return 1;
        }

        if ( !TestPassword( pPass, pUser ) ) {
            RequestAuthentication( sock, "FillInThisName" );
            return 1;
        }
    }
    return ( *oldhand ) ( sock, url, rxBuffer ); // Password was acceptable.
}

extern "C" void UserMain( void * pd )
{
    initWithWeb();

    iprintf("Application: %s\r\nNNDK Revision: %s\r\n",AppName,GetReleaseTag());

    // Register a new GET handler
    oldhand = SetNewGetHandler( MyDoGet );

    while (1) {
        OSTimeDly(20);
    }
}
