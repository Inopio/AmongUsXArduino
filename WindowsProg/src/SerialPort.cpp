#include "SerialPort.hpp"

Serial_Port::Serial_Port(std::string name) noexcept : name(std::move(name)) {
    platform.connected = false;

    platform.handle = CreateFileA(
        this->name.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (platform.handle == INVALID_HANDLE_VALUE) {
        printf(
            "ERROR: Handle was not attached. Reason: %s not available\n",
            this->name.c_str()
        );
        return;
    }

    DCB dcb = { 0 };

    if (!GetCommState(platform.handle, &dcb)) {
        printf(
            "ERROR: GetCommState failed on: %s\n",
            this->name.c_str()
        );
        return;
    }

    //frequence usb
    platform.baud_rate = CBR_9600;
    dcb.BaudRate = platform.baud_rate;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;

    if (!SetCommState(platform.handle, &dcb)) {
        printf(
            "ERROR: SetCommState failed on: %s\n",
            this->name.c_str()
        );
        return;
    }
    
    platform.connected = true;

    PurgeComm(platform.handle, PURGE_RXCLEAR | PURGE_TXCLEAR);
}

Serial_Port::~Serial_Port() noexcept {
    if (platform.connected) {
        platform.connected = false;
        CloseHandle(platform.handle);
    }
}

std::vector<uint8_t> Serial_Port::read(size_t n) noexcept {
    DWORD bytes_read{ 0 };

    ClearCommError(platform.handle, &platform.errors, &platform.status);

    if ((size_t)platform.status.cbInQue < n)
        n = (size_t)platform.status.cbInQue;

    std::vector<uint8_t> result(n);
    ReadFile(platform.handle, result.data(), n, &bytes_read, NULL);
    result.resize(bytes_read);

    return result;
}

std::vector<uint8_t> Serial_Port::wait_read(size_t n) noexcept {
    size_t bytes_read{ 0 };

    std::vector<uint8_t> result(n);
    while (bytes_read < n) {
        ClearCommError(platform.handle, &platform.errors, &platform.status);

        DWORD byte_read_this_time{ 0 };


        auto to_read = 
            ((size_t)platform.status.cbInQue < n - bytes_read)
                ? (size_t)platform.status.cbInQue
                : (n - bytes_read);
        ReadFile(
            platform.handle,
            result.data() + bytes_read,
            to_read,
            &byte_read_this_time,
            nullptr
        );
        bytes_read += byte_read_this_time;

        Sleep(100);
    }
    result.resize(bytes_read);
    return result;
}

size_t Serial_Port::write(const std::vector<uint8_t>& data) noexcept {
    DWORD send{ 0 };

    if (!WriteFile(platform.handle, (void*)data.data(), data.size(), &send, 0))
        ClearCommError(platform.handle, &platform.errors, &platform.status);
    
    return send;
}