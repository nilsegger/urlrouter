# URL Router
This URL-Router is a fast and simple way, to implement routing for your C/C++-Application. 
Use placeholder routes like "/users/{username}" and get your *username* variable set.

## Motivation
This project was created after the need of a simple, standalone and fast C / C++ routing library with placeholder functionalities.

## Code Example
```c
  // Create matchable route with "username" as fillable placeholder
  path_t* path = NULL;
  uint8_t res = create_path(&path, "/users/{username}", 1, '{', '}');
  
  // Match incoming request
  uint64_t matched;
  placeholder_t* placeholders;
  res = find_url(&matched, &placeholders, path, "/users/nils");
  
  // 'matched' set to 1
  // 'placeholders->name' set to "username"
  // 'placeholders->value' set to "nils"
```

## Installation
```
git clone https://github.com/nilsegger/urlrouter.git
cd urlrouter
mkdir build
cd build
cmake ..
make
make install
```

## TODO
- Query parameters support
- Decoding of URL special characters

## License

[Apache License 2.0](https://www.apache.org/licenses/LICENSE-2.0.html)
