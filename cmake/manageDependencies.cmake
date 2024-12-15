# spdlog
find_package(spdlog 1.12.0 REQUIRED)

# magic_enum
find_package(magic_enum 0.9.3 REQUIRED)

# nlohmann_json
find_package(nlohmann_json 3.11.2 REQUIRED)

# stduuid
find_package(stduuid 1.2.3 REQUIRED)

# sfml
find_package(
  SFML 2.6.0
  COMPONENTS graphic window system audio
  REQUIRED)

# rocket
find_package(rocket REQUIRED)

# chef-fun
find_package(chef-fun REQUIRED)
