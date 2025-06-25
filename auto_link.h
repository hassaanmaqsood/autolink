#ifndef AUTO_LINK_H
#define AUTO_LINK_H

#include <Arduino.h>
#include <functional>

class AutoReconnect
{
public:
    // Callback types for different events
    using ReconnectCallback = std::function<bool()>;           // Should return true if reconnect attempt was initiated
    using StatusCallback = std::function<bool()>;             // Should return true if currently connected
    using LogCallback = std::function<void(const String&)>;   // For logging messages
    using ErrorCallback = std::function<void(const String&)>; // For error messages

private:
    bool _shouldReconnect = true;
    bool _wasConnected = false;
    unsigned long _lastConnectAttempt = 0;
    unsigned long _connectionStartTime = 0;
    int _reconnectAttempts = 0;
    int _maxReconnectAttempts = 5;
    
    unsigned long _connectionTimeoutMs = 15000;  // 15 seconds
    unsigned long _reconnectDelayMs = 30000;     // 30 seconds
    
    // Callbacks
    ReconnectCallback _reconnectCallback = nullptr;
    StatusCallback _statusCallback = nullptr;
    LogCallback _logCallback = nullptr;
    ErrorCallback _errorCallback = nullptr;
    
    String _connectionName = "Connection";

public:
    AutoReconnect(const String& connectionName = "Connection") 
        : _connectionName(connectionName) {}

    // Configuration methods
    void setMaxAttempts(int maxAttempts) { _maxReconnectAttempts = maxAttempts; }
    void setConnectionTimeout(unsigned long timeoutMs) { _connectionTimeoutMs = timeoutMs; }
    void setReconnectDelay(unsigned long delayMs) { _reconnectDelayMs = delayMs; }
    void setConnectionName(const String& name) { _connectionName = name; }

    // Callback setters
    void setReconnectCallback(ReconnectCallback callback) { _reconnectCallback = callback; }
    void setStatusCallback(StatusCallback callback) { _statusCallback = callback; }
    void setLogCallback(LogCallback callback) { _logCallback = callback; }
    void setErrorCallback(ErrorCallback callback) { _errorCallback = callback; }

    // Control methods
    void enable() 
    { 
        _shouldReconnect = true; 
        _reconnectAttempts = 0;
        if (_logCallback) {
            _logCallback(_connectionName + " auto-reconnect enabled");
        }
    }
    
    void disable() 
    { 
        _shouldReconnect = false; 
        if (_logCallback) {
            _logCallback(_connectionName + " auto-reconnect disabled");
        }
    }
    
    void reset() 
    { 
        _reconnectAttempts = 0; 
        _lastConnectAttempt = 0;
        _connectionStartTime = 0;
    }

    // Called when a connection attempt starts
    void onConnectionAttemptStarted()
    {
        _connectionStartTime = millis();
        _lastConnectAttempt = millis();
    }

    // Called when connection status changes
    void onConnectionStatusChanged(bool connected)
    {
        if (connected && !_wasConnected) {
            // Just connected
            _reconnectAttempts = 0;
            _connectionStartTime = 0;
            if (_logCallback) {
                _logCallback(_connectionName + " connected successfully");
            }
        } else if (!connected && _wasConnected) {
            // Just disconnected
            if (_shouldReconnect) {
                _lastConnectAttempt = millis();
                if (_logCallback) {
                    _logCallback(_connectionName + " disconnected, will attempt reconnect");
                }
            }
        }
        _wasConnected = connected;
    }

    // Main loop - call this regularly
    void loop()
    {
        if (!_statusCallback || !_reconnectCallback) {
            return; // Callbacks not set
        }

        bool currentlyConnected = _statusCallback();
        
        // Update connection status
        if (currentlyConnected != _wasConnected) {
            onConnectionStatusChanged(currentlyConnected);
        }

        // Check connection timeout (commented out as in original)
        /*
        if (!currentlyConnected && _connectionStartTime > 0 &&
            millis() - _connectionStartTime > _connectionTimeoutMs)
        {
            if (_errorCallback) {
                _errorCallback(_connectionName + " connection timeout");
            }
            _connectionStartTime = 0;
        }
        */

        // Auto-reconnect logic
        if (!currentlyConnected && _shouldReconnect &&
            _reconnectAttempts < _maxReconnectAttempts &&
            millis() - _lastConnectAttempt > _reconnectDelayMs)
        {
            _reconnectAttempts++;
            _lastConnectAttempt = millis();
            _connectionStartTime = millis();

            if (_logCallback) {
                _logCallback(_connectionName + " reconnecting (" + 
                           String(_reconnectAttempts) + "/" + 
                           String(_maxReconnectAttempts) + ")");
            }
            
            // Attempt reconnection
            if (!_reconnectCallback()) {
                if (_errorCallback) {
                    _errorCallback(_connectionName + " reconnect attempt failed to start");
                }
            }
        }

        // Stop trying after max attempts
        if (_reconnectAttempts >= _maxReconnectAttempts && !currentlyConnected && _shouldReconnect)
        {
            _shouldReconnect = false;
            if (_errorCallback) {
                _errorCallback(_connectionName + " auto-reconnect disabled after max attempts");
            }
        }
    }

    // Getters
    bool isReconnectEnabled() const { return _shouldReconnect; }
    int getReconnectAttempts() const { return _reconnectAttempts; }
    int getMaxReconnectAttempts() const { return _maxReconnectAttempts; }
    unsigned long getLastAttemptTime() const { return _lastConnectAttempt; }
    bool wasConnected() const { return _wasConnected; }
};

#endif // AUTO_LINK_H
