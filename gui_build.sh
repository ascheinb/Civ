ATK_INC=/usr/local/Cellar/atk/2.36.0/include/atk-1.0
ATK_LIB=/usr/local/Cellar/atk/2.36.0/lib

ATKMM_INC=/usr/local/Cellar/atkmm/2.28.0_2/include/atkmm-1.6
ATKMM_LIB=/usr/local/Cellar/atkmm/2.28.0_2/lib

FREETYPE_INC=/usr/local/Cellar/freetype/2.10.2/include/freetype2
FREETYPE_LIB=/usr/local/Cellar/freetype/2.10.2/lib

SIGCPP_LIB_INC=/usr/local/Cellar/libsigc++@2/2.10.3/lib/sigc++-2.0/include
SIGCPP_INC=/usr/local/Cellar/libsigc++@2/2.10.3/include/sigc++-2.0
SIGCPP_LIB=/usr/local/Cellar/libsigc++@2/2.10.3/lib

GIOMM_LIB_INC=/usr/local/Cellar/glibmm/2.64.2/lib/giomm-2.4/include
GIOMM_INC=/usr/local/Cellar/glibmm/2.64.2/include/giomm-2.4
GIOMM_LIB=/usr/local/Cellar/glibmm/2.64.2/lib

GDK_PIXBUF_INC=/usr/local/Cellar/gdk-pixbuf/2.40.0_1/include/gdk-pixbuf-2.0
GDK_PIXBUF_LIB=/usr/local/Cellar/gdk-pixbuf/2.40.0_1/lib

GLIB_LIB_INC=/usr/local/Cellar/glib/2.64.3/lib/glib-2.0/include
GLIB_INC=/usr/local/Cellar/glib/2.64.3/include/glib-2.0
GLIB_LIB=/usr/local/Cellar/glib/2.64.3/lib

GLIBMM_INC=/usr/local/Cellar/glibmm/2.64.2/include/glibmm-2.4
GLIBMM_LIB=/usr/local/Cellar/glibmm/2.64.2/lib
GLIBMM_LIB_INC=/usr/local/Cellar/glibmm/2.64.2/lib/glibmm-2.4/include

CAIRO_INC=/usr/local/Cellar/cairo/1.16.0_3/include/cairo
CAIRO_LIB=/usr/local/Cellar/cairo/1.16.0_3/lib

CAIROMM_INC=/usr/local/Cellar/cairomm/1.12.2_1/include/cairomm-1.0
CAIROMM_LIB=/usr/local/Cellar/cairomm/1.12.2_1/lib
##CAIROMM_INC=/usr/local/Cellar/cairomm/1.12.2_1/include/cairomm-1.0

HARFBUZZ_INC=/usr/local/Cellar/harfbuzz/2.6.8/include/harfbuzz
HARFBUZZ_LIB=/usr/local/Cellar/harfbuzz/2.6.8/lib

PANGO_LIB=/usr/local/Cellar/pango/1.44.7/lib
PANGO_INC=/usr/local/Cellar/pango/1.44.7/include/pango-1.0

PANGOMM_LIB_INC=/usr/local/Cellar/pangomm/2.42.1/lib/pangomm-1.4/include
PANGOMM_INC=/usr/local/Cellar/pangomm/2.42.1/include/pangomm-1.4
PANGOMM_LIB=/usr/local/Cellar/pangomm/2.42.1/lib

GTK_LIB_INC=/usr/local/Cellar/gtk+3/3.24.21/lib
GTK_INC=/usr/local/Cellar/gtk+3/3.24.21/include/gtk-3.0
GTK_LIB=/usr/local/Cellar/gtk+3/3.24.21/lib

GDKMM_LIB_INC=/usr/local/Cellar/gtkmm3/3.24.2_1/lib/gdkmm-3.0/include
GDKMM_INC=/usr/local/Cellar/gtkmm3/3.24.2_1/include/gdkmm-3.0
GDKMM_LIB=/usr/local/Cellar/gtkmm3/3.24.2_1/lib

GTKMM_INC=/usr/local/Cellar/gtkmm3/3.24.2_1/include/gtkmm-3.0
GTKMM_LIB=/usr/local/Cellar/gtkmm3/3.24.2_1/lib
GTKMM_LIB_INC=/usr/local/Cellar/gtkmm3/3.24.2_1/lib/gtkmm-3.0/include

g++ -g -std=c++11 \
    -L${ATK_LIB} -I${ATK_INC} \
    -L${ATKMM_LIB} -I${ATKMM_INC} \
    -L${FREETYPE_LIB} -I${FREETYPE_INC} \
    -I${SIGCPP_LIB_INC} -L${SIGCPP_LIB} -I${SIGCPP_INC} \
    -I${GIOMM_LIB_INC} -L${GIOMM_LIB} -I${GIOMM_INC} \
    -L${GDK_PIXBUF_LIB} -I${GDK_PIXBUF_INC} \
    -I${GLIB_LIB_INC} -L${GLIB_LIB} -I${GLIB_INC} \
    -I${GLIBMM_LIB_INC} -L${GLIBMM_LIB} -I${GLIBMM_INC} \
    -L${CAIRO_LIB} -I${CAIRO_INC} \
    -L${CAIROMM_LIB} -I${CAIROMM_INC} \
    -L${HARFBUZZ_LIB} -I${HARFBUZZ_INC} \
    -L${PANGO_LIB} -I${PANGO_INC} \
    -I${PANGOMM_LIB_INC} -L${PANGOMM_LIB} -I${PANGOMM_INC} \
    -I${GTK_LIB_INC} -L${GTK_LIB} -I${GTK_INC} \
    -I${GDKMM_LIB_INC} -L${GDKMM_LIB} -I${GDKMM_INC} \
    -I${GTKMM_LIB_INC} -L${GTKMM_LIB} -I${GTKMM_INC} \
    main.cpp \
    -lgtkmm-3.0 -lgtk-3 -lgdkmm-3.0 -lgdk-3 -latk-1.0 -latkmm-1.6 -lsigc-2.0 -lglibmm-2.4 -lgio-2.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo-gobject -lpango-1.0 -lcairomm-1.0 -lcairo -lgobject-2.0 -lglib-2.0
