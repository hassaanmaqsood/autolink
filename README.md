# AutoLink

A lightweight, flexible auto-reconnection library for embedded systems and IoT devices. AutoLink automatically manages connection recovery for WiFi, Bluetooth, cellular, or any other connection type with configurable retry logic and comprehensive callback support.

## Features

- 🔄 **Automatic Reconnection** - Intelligent retry logic with configurable delays
- ⚙️ **Highly Configurable** - Customize timeouts, retry counts, and delays
- 📡 **Connection Agnostic** - Works with WiFi, Bluetooth, cellular, or custom connections
- 🔌 **Callback-Driven** - Flexible event handling through function callbacks
- 📊 **Status Monitoring** - Real-time connection status tracking
- 🪶 **Lightweight** - Minimal memory footprint, perfect for resource-constrained devices
- 🛡️ **Robust Error Handling** - Comprehensive error reporting and recovery

## Compatibility

- **Primary Target**: ESP32, ESP8266, Arduino-compatible boards
- **Secondary**: Any C++ environment with Arduino-style timing functions
- **Connections**: WiFi, Bluetooth, Ethernet, cellular, custom protocols

## Installation

### Arduino IDE
1. Download the library as ZIP
2. In Arduino IDE: `Sketch` → `Include Library` → `Add .ZIP Library`
3. Select the downloaded ZIP file

### PlatformIO
```ini
lib_deps = 
    https://github.com/hassaanmaqsood/AutoLink.git
```

### Manual Installation
1. Clone or download this repository
2. Copy the `AutoLink` folder to your libraries directory

## Quick Start

```cpp
#include <AutoLink.h>
#include <WiFi.h>

AutoLink wifiReconnect("WiFi");

// WiFi credentials
const char* ssid = "YourWiFiName";
const char* password = "YourPassword";

void setup() {
    Serial.begin(115200);
    
    // Configure AutoLink
    wifiReconnect.setMaxAttempts(10);
    wifiReconnect.setReconnectDelay(5000);  // 5 seconds between attempts
    
    // Set up callbacks
    wifiReconnect.setReconnectCallback([]() -> bool {
        Serial.println("Attempting WiFi reconnection...");
        WiFi.begin(ssid, password);
        return true;
    });
    
    wifiReconnect.setStatusCallback([]() -> bool {
        return WiFi.status() == WL_CONNECTED;
    });
    
    wifiReconnect.setLogCallback([](const String& message) {
        Serial.println("[AutoLink] " + message);
    });
    
    wifiReconnect.setErrorCallback([](const String& error) {
        Serial.println("[ERROR] " + error);
    });
    
    // Enable auto-reconnection
    wifiReconnect.enable();
    
    // Initial connection
    WiFi.begin(ssid, password);
}

void loop() {
    wifiReconnect.loop();  // Call this regularly!
    
    // Your main application code here
    delay(1000);
}
```

## API Reference

### Constructor
```cpp
AutoLink(const String& connectionName = "Connection")
```

### Configuration Methods
```cpp
void setMaxAttempts(int maxAttempts);           // Default: 5
void setConnectionTimeout(unsigned long ms);    // Default: 15000ms
void setReconnectDelay(unsigned long ms);       // Default: 30000ms
void setConnectionName(const String& name);     // For logging
```

### Callback Registration
```cpp
void setReconnectCallback(ReconnectCallback callback);
void setStatusCallback(StatusCallback callback);
void setLogCallback(LogCallback callback);
void setErrorCallback(ErrorCallback callback);
```

#### Callback Types
- **ReconnectCallback**: `std::function<bool()>` - Return `true` if reconnect attempt was initiated
- **StatusCallback**: `std::function<bool()>` - Return `true` if currently connected
- **LogCallback**: `std::function<void(const String&)>` - Receive log messages
- **ErrorCallback**: `std::function<void(const String&)>` - Receive error messages

### Control Methods
```cpp
void enable();      // Enable auto-reconnection
void disable();     // Disable auto-reconnection
void reset();       // Reset attempt counters
void loop();        // Main processing - call regularly!
```

### Status Methods
```cpp
bool isReconnectEnabled() const;
int getReconnectAttempts() const;
int getMaxReconnectAttempts() const;
unsigned long getLastAttemptTime() const;
bool wasConnected() const;
```

### Event Notification
```cpp
void onConnectionAttemptStarted();          // Call when starting connection
void onConnectionStatusChanged(bool connected); // Call when status changes
```

## Advanced Examples

### WiFi with Custom Retry Logic
```cpp
AutoLink wifiLink("WiFi-Advanced");

void setupAdvancedWiFi() {
    wifiLink.setMaxAttempts(15);
    wifiLink.setReconnectDelay(10000);  // 10 seconds
    
    wifiLink.setReconnectCallback([]() -> bool {
        // Custom reconnection logic
        WiFi.disconnect();
        delay(100);
        WiFi.begin(ssid, password);
        
        // Optional: Try different access points
        static int apIndex = 0;
        const char* backupAPs[] = {"WiFi-Main", "WiFi-Backup"};
        WiFi.begin(backupAPs[apIndex % 2], password);
        apIndex++;
        
        return true;
    });
    
    wifiLink.enable();
}
```

### Bluetooth Connection Management
```cpp
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;
AutoLink btLink("Bluetooth");

void setupBluetooth() {
    btLink.setReconnectCallback([]() -> bool {
        SerialBT.begin("ESP32-Device");
        return true;
    });
    
    btLink.setStatusCallback([]() -> bool {
        return SerialBT.connected();
    });
    
    btLink.enable();
}
```

### Multiple Connection Management
```cpp
AutoLink wifiLink("WiFi");
AutoLink mqttLink("MQTT");

void loop() {
    wifiLink.loop();
    mqttLink.loop();
    
    // Your application logic
}
```

## Best Practices

1. **Always call `loop()`** - Place `autoLink.loop()` in your main loop
2. **Set appropriate delays** - Balance between responsiveness and resource usage
3. **Use meaningful connection names** - Helps with debugging and logging
4. **Handle errors gracefully** - Implement error callbacks for robust operation
5. **Monitor connection status** - Use status callbacks for application logic
6. **Reset when needed** - Call `reset()` after manual reconnections

## Troubleshooting

**Connection not auto-reconnecting?**
- Ensure `loop()` is called regularly
- Check that callbacks are properly set
- Verify `enable()` was called

**Too many reconnection attempts?**
- Adjust `setMaxAttempts()` value
- Increase `setReconnectDelay()` for less aggressive retries

**Memory issues?**
- AutoLink has minimal footprint, but check callback implementations
- Avoid memory allocation in callbacks

## Contributing

Contributions are welcome! Please feel free to submit pull requests, report bugs, or suggest features.

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE.md) file for details.

## Changelog

### v1.0.0
- Initial release
- Basic auto-reconnection functionality
- Callback-based architecture
- Configurable retry logic

---

**Made with ❤️ for the IoT community**

*If AutoLink helps your project, please consider giving it a star! ⭐*
*or just contribute to make it better*
