#include <QApplication>
#include <QStyle>
#include "main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    
    // Set application style
    app.setStyle("Fusion");
    
    // Set application palette (dark theme)
    QPalette dark_palette;
    dark_palette.setColor(QPalette::Window, QColor(26, 26, 26));
    dark_palette.setColor(QPalette::WindowText, QColor(236, 240, 241));
    dark_palette.setColor(QPalette::Base, QColor(44, 62, 80));
    dark_palette.setColor(QPalette::AlternateBase, QColor(52, 73, 94));
    dark_palette.setColor(QPalette::ToolTipBase, QColor(236, 240, 241));
    dark_palette.setColor(QPalette::ToolTipText, QColor(26, 26, 26));
    dark_palette.setColor(QPalette::Text, QColor(236, 240, 241));
    dark_palette.setColor(QPalette::Button, QColor(44, 62, 80));
    dark_palette.setColor(QPalette::ButtonText, QColor(236, 240, 241));
    dark_palette.setColor(QPalette::BrightText, Qt::red);
    dark_palette.setColor(QPalette::Link, QColor(52, 152, 219));
    dark_palette.setColor(QPalette::Highlight, QColor(52, 152, 219));
    dark_palette.setColor(QPalette::HighlightedText, Qt::black);
    
    app.setPalette(dark_palette);
    
    // Create main window (widget UI - always available)
    MainWindow window;
    window.show();
    
    // Optional: QML UI can be added in future (requires Qt6 Qml/Quick packages)
    
    return app.exec();
}
