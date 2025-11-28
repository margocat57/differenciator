import matplotlib.pyplot as plt
import numpy as np

x = np.arange(-10, 10.01, 0.01) # задание оси икс - подумать как сделать
y = np.sin(x) # вот это будет дампится

plt.plot(x, y)

plt.title('График синуса')
plt.xlabel('X')
plt.ylabel('sin(X)')
plt.grid(True) # Добавить сетку

plt.show()