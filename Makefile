# === Compiler & Flags ===
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

# === Projektdateien ===
SRCS = src/main.cpp src/ui.cpp src/geo.cpp
TARGET = geofetch

# === Installationsverzeichnis (User-local!) ===
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin

# === Default-Ziel: compile + build ===
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# === Installiere das Executable nach ~/.local/bin ===
install: geofetch
	sudo cp geofetch $(BINDIR)/
	sudo chmod +x $(BINDIR)/geofetch
	@echo "✅ Installed to $(BINDIR)/geofetch"

# === Entferne das Executable aus ~/.local/bin ===
uninstall:
	@rm -f $(BINDIR)/$(TARGET)
	@echo "🗑️  Uninstalled from $(BINDIR)"

# === Lösche erzeugte Binaries im Projektverzeichnis ===
clean:
	@rm -f $(TARGET)
	@echo "🧹 Cleaned build artifacts"

# === Optional: direkt ausführen (z. B. für dev) ===
run: $(TARGET)
	./$(TARGET)