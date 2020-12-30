struct Control{
    std::string question = "\n\n\n";
    bool active=true;
    bool needs_float=false;
    float input_float=0.0f;
    float floatmin=0.0f;
    float floatmax=0.0f;
    bool needs_int=false;
    int input_int=0;
    int intmin=0;
    int intmax=0;
    bool range=false;
    bool needs_bool=false;
    bool input_bool=false;
    int info_id=0; // Which person we're focused on
};

Control ctrl;
