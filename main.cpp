#include "ui/menu.h"
#include "core/logger.h"
#include "core/config.h"

int main() {
    Config& cfg = Config::instance();
    Logger::instance().set_file("poker.log");
    cfg.load("config.ini");
    if (cfg.log_file != "poker.log") Logger::instance().set_file(cfg.log_file);
    LOG_INFO("=== Game started ===");
    run_main_menu();
    LOG_INFO("=== Game exited ===");
    return 0;
}
