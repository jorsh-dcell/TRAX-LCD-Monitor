<img width="900" height="1600" alt="image" src="https://github.com/user-attachments/assets/d468230c-8426-486f-9c4b-f0eeebfc61b1" />
<img width="900" height="1600" alt="imagen_2026-09-15_101236347" src="https://github.com/user-attachments/assets/e564a4f9-7bb5-4af4-89cc-d372ab080f32" />








## 🔌 Diagrama de Conexiones 

```text
       [ PANTALLA LCD 16x2 ]             [ ARDUINO UNO / NANO ]
       ┌───────────────────┐             ┌────────────────────┐
       │             [GND] ├─────────────┤ [GND]              │
       │             [VCC] ├─────────────┤ [5V]               │
       │  Adaptador  [SDA] ├─────────────┤ [A4]               │
       │   I2C (4 pins)    │             │                    │
       │             [SCL] ├─────────────┤ [A5]               │
       └───────────────────┘             └────────────────────┘
```




# TRAX-LCD-Monitor
Monitor de recursos de PC y ecualizador inteligente automático para LCD 16x2 usando Arduino y Python.



