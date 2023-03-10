# AnnaDB CPP-Client

[![AnnaDB Logo](https://annadb.dev/assets/img/logo_colored.svg)](https://annadb.dev)![And](https://cdn.iconscout.com/icon/free/png-128/mobile-keyboard-key-program-ampersand-and-11559.png)![ISO_C++_Logo.svg.png](..%2Fimages%2FISO_C%2B%2B_Logo.svg.png)
This is a C++ driver built for type-safe query creation.

AnnaDB uses a custom query language called [TySON](https://github.com/roman-right/tyson)


## Required
- [zeromq](https://zeromq.org/download/)
- cmake >= 3.24


## Test library

- googletest

## Docs
- generated via `doxygen` under [docs](https://anna-team.github.io/cpp_driver/html/annotated.html)

## local development
- create a AnnaDB instance via `docker-compose up` access the DB via port `10001` on `localhost`