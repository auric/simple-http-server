Documentation for simple-http-server
===================

## Build
To build the app, use `cmake` on a Linux platform, as the app relies on Linux sockets.
You can create a directory named `build-simple-http-server` near the cloned repository to generate the build files within it.
Use the path to the original repository in the `cmake` command to generate the `make` files for the project.
Finally, employ the `make` command to build the app.

```
mkdir build-simple-http-server
cd build-simple-http-server
cmake ../simple-http-server
make
```

##Design
`Handler` subclasses are employed for HTTP methods, and they possess the knowledge of how to process each request.
These handlers collaborate with the `Database` class, which, in turn, knows how to save and retrieve various versions of requests.
In the case of a `POST` request, a new version of data is appended, while the `DELETE` method is used to remove the latest version.

Additionally, an `HTTPServer` class exists, which encapsulates the client connection and the handlers for these methods and invokes the process function for each of them.
I did't use `std::thread` because I believe it is not necessary in the app.

##Possible improvements
Using `stringstream` to process request headers was a poor choice due to performance issues.
It is more efficient to utilize a regular `std::string` with the `find` method.
Similarly, regular expressions should be avoided, as they can be quite slow in C++.

