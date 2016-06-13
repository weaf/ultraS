
avg = 48.40
far = 550
too_close = 40

avg_remap = far + too_close - avg
k = 1/far

k_avg = avg_remap * k

speed = k_avg * 255

 %avg 48.40 avg_remap 541.60 distance 0 speed 1
