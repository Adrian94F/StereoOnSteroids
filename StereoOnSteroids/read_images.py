import numpy as np
import cv2

import click

from time import sleep


def main():
    cap1 = cv2.VideoCapture(1)  # Camera 1
    cap2 = cv2.VideoCapture(2)  # Camera 2
    imgs_directory = "calib_imgs/0/"
    extension = "jpg"
    i = 0

    #@click.command()
    #@click.option('--imwidth', default=1, prompt='Image width')
    #@click.option('--imheight', default=1, prompt='Image height')
    #@click.option('imgs_directory', prompt="Directory to save images in")
    #@click.option('extension', prompt='Image extension')
    while True:
        # Capturing frame by frame
        ret, img1 = cap1.read()  # Capture frame by frame from Camera 1
        ret, img2 = cap2.read()  # Capture frame by frame from Camera 2
        #r1 = imwidth / img1.shape[1]  # Dividing by the width
        #ratio1 = (imwidth, int(img1.shape[0] * r1))
        # multiplying by the height/width
        #r2 = imwidth / img2.shape[1]
        #ratio2 = (imwidth, int(img2.shape[1] * r2))
        #img_res1 = cv2.resize(img1, ratio1, interpolation=cv2.INTER_AREA)
        # INTER_AREA optimum for scaling down
        #img_res2 = cv2.resize(img2, ratio2, interpolation=cv2.INTER_AREA)
        cv2.imshow("IMG1", img1)
        cv2.imshow("IMG2", img2)
        wk = cv2.waitKey(30)
        if (wk == 27):
            return
        if (wk > 0):
            print("Saving img pair ".format(i))
            filename1 = "{}left{}.{}".format(imgs_directory, i, extension)
            filename2 = "{}right{}.{}".format(imgs_directory, i, extension)
            print(filename1)
            cv2.imwrite(filename1, img1)
            cv2.imwrite(filename2, img2)
            i += 1


if __name__ == '__main__':
    main()

