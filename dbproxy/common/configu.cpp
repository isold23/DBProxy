

int CConfigure::parse_array(const char* value, std::vector<std::string>& array)
{
    int len = strlen(value);
        
            if(value[0] != '{' && value[len - 1] != '}') {
                    TRACE(2, "value format error. value: " << value);
                            return -1;
                                }
                                    
                                        char buffer[1024];
                                            memset(buffer, 0, 1024);
                                                char* start = (char*)(value + 1);
                                                    char* end = (char*)(value + 1);
                                                        
                                                            while(end != '\0') {
                                                                    if(*end == ',' || *end == '}') {
                                                                                int a = end - start;
                                                                                            memcpy(buffer, start, a);
                                                                                                        array.push_back(buffer);
                                                                                                                    memset(buffer, 0, 1024);
                                                                                                                                start = end + 1;
                                                                                                                                        }
                                                                                                                                                
                                                                                                                                                        if(*end == '}') {
                                                                                                                                                                    break;
                                                                                                                                                                            }
                                                                                                                                                                                    
                                                                                                                                                                                            end++;
                                                                                                                                                                                                }
                                                                                                                                                                                                    
                                                                                                                                                                                                        return 0;
                                                                                                                                                                                                        }


