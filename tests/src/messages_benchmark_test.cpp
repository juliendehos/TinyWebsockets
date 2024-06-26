#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <tiny_websockets/client.hpp>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif

/* Remove if already defined */
typedef long long int64; typedef unsigned long long uint64;

/* Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both
 * windows and linux. */

uint64 GetTimeMs64()
{
#ifdef _WIN32
 /* Windows */
 FILETIME ft;
 LARGE_INTEGER li;

 /* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
  * to a LARGE_INTEGER structure. */
 GetSystemTimeAsFileTime(&ft);
 li.LowPart = ft.dwLowDateTime;
 li.HighPart = ft.dwHighDateTime;

 uint64 ret = li.QuadPart;
 ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
 ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

 return ret;
#else
 /* Linux */
 struct timeval tv;

 gettimeofday(&tv, NULL);

 uint64 ret = tv.tv_usec;
 /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
 ret /= 1000;

 /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
 ret += (tv.tv_sec * 1000);

 return ret;
#endif
}

using namespace websockets;

TEST_CASE( "Benchmarking message sending to server" ) {
  WebsocketsClient client;
    REQUIRE( client.connect("localhost", 8080, "/") == true );
    volatile bool done = false;

    client.onMessage([&done](WebsocketsClient& client, auto message){
        REQUIRE( message.data() == "ECHO: Hello Server" );
        done = true;
    });

    const auto ITERATIONS = 1000000;

    auto beg = GetTimeMs64();
    for(auto i = 0; i < ITERATIONS; ++i) {
        client.send("Hello Server");
    }
    auto end = GetTimeMs64();
    std::cout << "Time For " << ITERATIONS << ": " << (end - beg) << "ms, Average: " << (end - beg) / (ITERATIONS * 1.0) << std::endl;

    //while(!done) client.poll();
    client.close();
}
