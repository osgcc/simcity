#pragma once

#include <string>

typedef char** Handle;

std::string& GetResource(const std::string& name, int id);
const std::string GetIndString(const std::string&, int id, int num);
