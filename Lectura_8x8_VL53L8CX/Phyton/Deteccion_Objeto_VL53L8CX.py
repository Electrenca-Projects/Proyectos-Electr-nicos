import serial
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches

# -------------------------------------------------------------------------
# 1. Configuración Inicial
# -------------------------------------------------------------------------

# Configura el puerto serial (Asegúrate de cambiar 'COM7' al puerto correcto)
ser = serial.Serial('COM7', 115200, timeout=1)

# Configuración del display
plt.ion() # Modo interactivo
fig, ax = plt.subplots()
data_matrix = np.zeros((8, 8))
im = ax.imshow(data_matrix, cmap='viridis', vmin=0, vmax=2000)
plt.colorbar(im, ax=ax, label="Distancia (mm)")

# Parámetros de detección
threshold = 80         # Cambio mínimo (en mm) para considerar detección
calibration_reads = 10 # lecturas para calibración inicial
calibration_matrix = None
Rect = None # Variable para dibujar el cuadrado del objeto


# -------------------------------------------------------------------------
# 2. Calibración Inicial
# -------------------------------------------------------------------------

print("Calibrando...")
calibration_sum = np.zeros((8, 8))

for _ in range(calibration_reads):
    matrix = None
    while matrix is None:
        line_data = []
        while True:
            # Lee una línea y decodifica/limpia
            line = ser.readline().decode(errors="ignore").strip()
            
            if line == "END":
                break # Marca de fin de bloque del microcontrolador
            
            if line:
                # Convierte la línea a una fila de enteros (asume 8 valores)
                row = [int(x) for x in line.split()]
                if len(row) == 8:
                    line_data.append(row)
        
        # Si se leyeron las 8 filas (matriz 8x8), la convierte a array de NumPy
        if len(line_data) == 8:
            matrix = np.array(line_data)

    calibration_sum += matrix

# Calcula el promedio de las lecturas para la matriz de fondo
calibration_matrix = calibration_sum / calibration_reads
print("Calibración completada")


# -------------------------------------------------------------------------
# 3. Bucle Principal (Detección y Visualización)
# -------------------------------------------------------------------------

while True:
    try:
        # --- Leer Matriz de Datos Actual ---
        matrix = None
        while matrix is None:
            line_data = []
            while True:
                line = ser.readline().decode(errors='ignore').strip()
                if line == "END":
                    break
                if line:
                    row = [int(x) for x in line.split()]
                    if len(row) == 8:
                        line_data.append(row)
            
            if len(line_data) == 8:
                matrix = np.array(line_data)

        # --- Detectar movimiento ---
        # Crea una máscara booleana: True donde la diferencia con la calibración > threshold
        movement_mask = np.abs(matrix - calibration_matrix) > threshold

        # --- Crear Display ---
        # Muestra la matriz actual, enmascarando los píxeles sin movimiento
        display_matrix = np.ma.masked_where(~movement_mask, matrix)
        im.set_data(display_matrix)

        # --- Borrar cuadrado anterior ---
        if Rect:
            Rect.remove()
            Rect = None

        # --- Marcar el objeto si hay detección ---
        if np.any(movement_mask):
            pos = np.argwhere(movement_mask)
            # Calcula la celda promedio del movimiento
            row_avg = int(np.mean(pos[:,0]))
            col_avg = int(np.mean(pos[:,1]))

            # Dibuja un cuadrado rojo (patch) sobre la celda detectada
            Rect = patches.Rectangle((col_avg-0.5, row_avg-0.5), 1, 1, linewidth=2, edgecolor='red', facecolor='none')
            ax.add_patch(Rect)
        
        # --- Actualizar Gráfico ---
        ax.set_title("Detección de objeto VL53L8CX")
        plt.draw()
        plt.pause(0.01)

    except KeyboardInterrupt:
        print("\nFinalizado por usuario")
        ser.close() # Cierra el puerto serial al terminar
        break
    except Exception as e:
        print("Error:", e)