//run this after checking SD card is working with the SD example in the SD library

#include <SPI.h>
#include <SD.h>
#include <Arduino.h>
#include <driver/i2s.h>

// SD card configuration
#define SD_CS_PIN 5

// I2S configuration
#define I2S_WS 26 // LRCK or LRC on many I2S boards
#define I2S_SD 25 // DOUT - connect to DIN on the I2S board
#define I2S_SCK 27 // BCLK on many I2S boards

void setup() {
  Serial.begin(115200);

  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Failed to initialize SD card");
    return;
  }

  // Initialize I2S
  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_SD,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_set_clk(I2S_NUM_0, 44100, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

  playWavFile("/123_16bit_44khz.wav");
  playWavFile("/123_16bit_16khz.wav");
  playWavFile("/123_8bit_16khz.wav");
}

void loop() {
  // Empty loop as playback is handled in playWavFile()
}

void playWavFile(const char* filename) {
  File audioFile = SD.open(filename);
  if (!audioFile) {
    Serial.println("Failed to open audio file");
    return;
  }

  // Read WAV header
  if (!readWavHeader(audioFile)) {
    Serial.println("Failed to read WAV header");
    audioFile.close();
    return;
  }

  // Buffer to hold audio data
  uint8_t buffer[64];
  size_t bytesRead;

  while (audioFile.available()) {
    bytesRead = audioFile.read(buffer, sizeof(buffer));
    size_t bytesWritten;
    i2s_write(I2S_NUM_0, buffer, bytesRead, &bytesWritten, portMAX_DELAY);
  }

  audioFile.close();
}

bool readWavHeader(File& file) {
  char header[44];
  if (file.read((uint8_t*)header, 44) != 44) {
    Serial.println("Invalid WAV file");
    return false;
  }

  // Check "RIFF" and "WAVE" chunks
  if (strncmp(header, "RIFF", 4) != 0 || strncmp(header + 8, "WAVE", 4) != 0) {
    Serial.println("Invalid WAV file");
    return false;
  }

  // Check audio format (should be PCM)
  int audioFormat = header[20] | (header[21] << 8);
  if (audioFormat != 1) {
    Serial.println("Unsupported WAV format");
    return false;
  }

  // Check number of channels (should be 1 or 2)
  int numChannels = header[22] | (header[23] << 8);
  if (numChannels != 1 && numChannels != 2) {
    Serial.println("Unsupported number of channels");
    return false;
  }

  // Check sample rate
  int sampleRate = header[24] | (header[25] << 8) | (header[26] << 16) | (header[27] << 24);
  if (sampleRate != 44100) {
    Serial.println("Unsupported sample rate");
    return false;
  }

  // Check bits per sample
  int bitsPerSample = header[34] | (header[35] << 8);
  if (bitsPerSample != 16) {
    Serial.println("Unsupported bits per sample");
    return false;
  }

  return true;
}
