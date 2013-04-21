#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0xB4, 0x16, 0x47, 0x01, 0x9D, 0x04, 0x41, 0xF8, 0xB3, 0x9B, 0xF9, 0xEF, 0x3A, 0x4C, 0x14, 0xCF }
PBL_APP_INFO(MY_UUID,
             "Chatty", "Frank O'Dwyer",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
TextLayer label;
GFont biggestfont;
GFont bigfont;

static void update_time(PblTm* t) {
    static char buf[100];
    strcpy(buf , "It's ");
    int fuzzyresid = t->tm_min % 5;
    int fuzzymin = 5*(t->tm_min / 5);
    int hour = t->tm_hour;
    switch(fuzzyresid) {
        case 0:
            break;
        case 1:
            strcat(buf, "just gone ");
            break;
        case 2:
            strcat(buf, "gone ");
            break;
        case 3:
            strcat(buf, "going on ");
            fuzzymin = (fuzzymin + 5) % 60;
            if (fuzzymin==0) {
                hour = (hour + 1) % 24;
            }
            break;
        case 4:
        default:
            strcat(buf, "just going on ");
            fuzzymin = (fuzzymin + 5) % 60;
            if (fuzzymin==0) {
                hour = (hour + 1) % 24;
            }
            break;
    }

    text_layer_set_font(&label, bigfont);

    if (fuzzymin == 15 || fuzzymin == 45) {
        strcat(buf, "quarter ");
    } else if (fuzzymin == 30) {
        strcat(buf, "half ");
    } else {
        switch(fuzzymin) {
            case 0:
            default:
                break;
            case 5:
            case 55:
                strcat(buf, "five ");
                break;
            case 10:
            case 50:
                strcat(buf, "ten ");
                break;
            case 20:
            case 40:
                strcat(buf, "twenty ");
                break;
            case 25:
            case 35:
                strcat(buf, "twenty five ");
                break;
        }
    }

    if (fuzzymin > 0 && fuzzymin <= 30) {
        strcat(buf, "past ");
    } else if (fuzzymin > 0) {
        // there are two hard problems in computer science:
        // caching, naming, and off by one errors.
        hour = (hour + 1) % 24;
        strcat(buf, "to ");
    }

    switch(hour) {
        case 0:
            strcat(buf, "midnight");
            break;
        case 1:
        case 13:
            strcat(buf, "one");
            break;
        case 2:
        case 14:
            strcat(buf, "two");
            break;
        case 3:
        case 15:
            strcat(buf, "three");
            break;
        case 4:
        case 16:
            strcat(buf, "four");
            break;
        case 5:
        case 17:
            strcat(buf, "five");
            break;
        case 6:
        case 18:
            strcat(buf, "six");
            break;
        case 7:
        case 19:
            strcat(buf, "seven");
            break;
        case 8:
        case 20:
            strcat(buf, "eight");
            break;
        case 9:
        case 21:
            strcat(buf, "nine");
            break;
        case 10:
        case 22:
            strcat(buf, "ten");
            break;
        case 11:
        case 23:
            strcat(buf, "eleven");
            break;
        case 12:
            strcat(buf, "twelve");
            break;
    }

    if (fuzzymin == 0 && hour != 0) {
        strcat(buf, " o'clock");
    }

    if (strlen(buf) <= strlen("it's twelve o'clock")) {
        text_layer_set_font(&label, biggestfont);
    } else {
        text_layer_set_font(&label, bigfont);
    }

    text_layer_set_text(&label, buf);
}

static void handle_minute_tick(AppContextRef app_ctx, PebbleTickEvent* e) {
    update_time(e->tick_time);
}

void handle_init(AppContextRef ctx) {
    (void) ctx;
    
    window_init(&window, "Chatty");
    const bool animated = true;
    window_stack_push(&window, animated);
    
    window_set_background_color(&window, GColorBlack);
    bigfont = fonts_get_system_font(FONT_KEY_GOTHAM_30_BLACK);
    biggestfont = fonts_get_system_font(FONT_KEY_GOTHAM_42_BOLD);
    
    text_layer_init(&label, GRect(0, 0, window.layer.frame.size.w, window.layer.frame.size.h));
    text_layer_set_background_color(&label, GColorBlack);
    text_layer_set_text_color(&label, GColorWhite);
    text_layer_set_font(&label, bigfont);
    layer_add_child(&window.layer, &label.layer);
    
    PblTm t;
    get_time(&t);
    update_time(&t);
}


void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }
    };
    app_event_loop(params, &handlers);
}
