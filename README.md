# web-C
A collection of simple C programs which access web APIs intended for use on embedded devices. 

### Why C?
There are several languages which make accessing Web services trivial such as PHP, Python, Ruby, or even the lightweight Lua. However, on an embedded device where storage size and processor speed are at a premium, using a high-level scripting language can be very expensive. For one, just installing a language can occupy several megabytes of storage (again were talking embedded devices where the total storage size is 12 MB or 16 MB, for example). Second, installing the dependencies for these languages can be a nightmare… In most cases embedded devices run a Linux kernel with a limited set of packages compared to what you would expect on a desktop version, such as Ubuntu or Debian. Finally, if you manage to get the language in all its dependencies to fit on your device, you may find that your processor is completely tied up running your script.

Therefore the most efficient and performant implementation is to write the program in low-level C code. Not surprisingly, this is also the most difficult as you have to concern yourself with cross compiling your source code as well as any libraries which the source may call and ensuring that those libraries are also installed on the device. However, if you can jump through those hoops, you have a program which is both lightweight and relatively fast.

As the market for the Internet of Things (IoT) expands, so too will the proliferation of devices which are both:
1. Supercheap (and also very resource constrained), and
2. access the Internet (via WiFi, Bluetooth etc).
I hope this source is useful, if you have any suggestions for improvement **please feel free to send a pull request** and I will definitely review it.

### Libraries
These programs depend on the following libraries:
- [libcurl](https://github.com/bagder/curl) for making API calls to the web.
- [json-c](https://github.com/json-c/json-c) for parsing JSON responses.
- [liboauth](https://github.com/x42/liboauth) for authentication with services like Twitter.

### Programs
- Fetch weather data from Yahoo API
- Fetch home timeline from Twitter
