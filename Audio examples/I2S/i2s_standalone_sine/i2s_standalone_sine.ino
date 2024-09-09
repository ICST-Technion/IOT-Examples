#include <Arduino.h>
#include <driver/i2s.h>
#include <math.h>

// I2S configuration
#define I2S_WS 26   //LRC on max98357a
#define I2S_SD 25   //serial data - DIN on audio device, DOUT on ESP32
#define I2S_SCK 27  //BCLK on max98357a

const int sampleRate = 16000;  // Sample rate in Hz
float frequency = 440.0;       // Frequency of the sine wave in Hz (A4 note)
const int amplitude = 32767;   // Amplitude of the sine wave (16-bit max value)

void setup() {
  Serial.begin(115200);

  // Initialize I2S
  i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = sampleRate,
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
  i2s_set_clk(I2S_NUM_0, sampleRate, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);

  generateSineWave();
}

void loop() {
  // Empty loop as sine wave generation is handled in generateSineWave()
}

void generateSineWave() {
  int16_t sample;
  float phase = 0.0;
  float frequencyIncrement = 5;
  while (true) {
    float phaseIncrement = 2.0 * PI * frequency / sampleRate;

    for (int i = 0; i < 64; i++) {                 // Buffer size of 64 samples
      sample = (int16_t)(amplitude * sin(phase));  // Generate sine wave sample
      phase += phaseIncrement;
      if (phase >= 2.0 * PI) {
        phase -= 2.0 * PI;
      }

      int16_t buffer[2];   // Stereo buffer (left and right channels)
      buffer[0] = sample;  // Left channel
      buffer[1] = sample;  // Right channel

      size_t bytesWritten;
      i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytesWritten, portMAX_DELAY);
    }
    frequency += frequencyIncrement;
    if (frequency > 1000) {
      frequency = 440.0;
    }
  }
}
