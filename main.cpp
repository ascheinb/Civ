#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <gtkmm/application.h>

#include "civwindow.hpp"

int main(){
    // Initialize random seed
    srand (time(NULL));
    //srand(5);

    // Read in names
    fill_names();

    // Initialize default setup parameters
    SetupParameters setup_params;

    // Launch window
    auto app = Gtk::Application::create();
    CivWindow window(setup_params);
    return app->run(window);
}
