import time

while True:
    st = time.time()
    time.sleep(0.03)
    et = time.time()
    print('dt={dt}'.format(dt=et - st))
