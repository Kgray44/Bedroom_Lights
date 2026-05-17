#ifndef BEDROOM_LED_CONTROLLER_JSON_HELPERS_H
#define BEDROOM_LED_CONTROLLER_JSON_HELPERS_H

int findJsonValueStart(const String& json, const char* key) {
  String marker = String("\"") + key + "\":";
  int start = json.indexOf(marker);
  if (start < 0) {
    return -1;
  }
  start += marker.length();
  while (start < static_cast<int>(json.length()) && isspace(json[start])) {
    start++;
  }
  return start;
}

int findMatchingJsonClose(const String& json, int start, char openChar, char closeChar) {
  int depth = 0;
  bool inString = false;
  bool escaped = false;
  for (int i = start; i < static_cast<int>(json.length()); i++) {
    char c = json[i];
    if (inString) {
      if (escaped) {
        escaped = false;
      } else if (c == '\\') {
        escaped = true;
      } else if (c == '"') {
        inString = false;
      }
      continue;
    }
    if (c == '"') {
      inString = true;
    } else if (c == openChar) {
      depth++;
    } else if (c == closeChar) {
      depth--;
      if (depth == 0) {
        return i;
      }
    }
  }
  return -1;
}

bool extractJsonObjectForKey(const String& json, const char* key, String& objectJson) {
  int start = findJsonValueStart(json, key);
  if (start < 0) {
    return false;
  }
  start = json.indexOf('{', start);
  if (start < 0) {
    return false;
  }
  int end = findMatchingJsonClose(json, start, '{', '}');
  if (end < 0) {
    return false;
  }
  objectJson = json.substring(start, end + 1);
  return true;
}

bool extractJsonArrayForKey(const String& json, const char* key, String& arrayJson) {
  int start = findJsonValueStart(json, key);
  if (start < 0) {
    return false;
  }
  start = json.indexOf('[', start);
  if (start < 0) {
    return false;
  }
  int end = findMatchingJsonClose(json, start, '[', ']');
  if (end < 0) {
    return false;
  }
  arrayJson = json.substring(start, end + 1);
  return true;
}

bool extractFirstJsonObject(const String& json, String& objectJson) {
  int start = json.indexOf('{');
  if (start < 0) {
    return false;
  }
  int end = findMatchingJsonClose(json, start, '{', '}');
  if (end < 0) {
    return false;
  }
  objectJson = json.substring(start, end + 1);
  return true;
}

void copyFixedCString(char* destination, size_t destinationSize, const char* value) {
  if (destination == nullptr || destinationSize == 0) {
    return;
  }
  if (value == nullptr) {
    destination[0] = '\0';
    return;
  }
  strncpy(destination, value, destinationSize - 1);
  destination[destinationSize - 1] = '\0';
}

void copyFixedString(char* destination, size_t destinationSize, const String& value) {
  copyFixedCString(destination, destinationSize, value.c_str());
}

bool fixedEquals(const char* value, const String& expected) {
  if (value == nullptr) {
    return expected.length() == 0;
  }
  return strcmp(value, expected.c_str()) == 0;
}

String fixedString(const char* value) {
  return value == nullptr ? String("") : String(value);
}

#endif
