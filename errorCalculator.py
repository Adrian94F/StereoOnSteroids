import cv2

filename_single = "singlethread.png"
filename_multi = "multithread_{}.png"
expected_height = 422

def fixImage(image):
    height = image.shape[0]
    if height == expected_height:
        #print (" correct height")
        return image
    if height > expected_height:
        crop_top = int((height - expected_height) / 2)
        image = image[crop_top:expected_height+crop_top, 0:image.shape[1]]
        #print("  cropped: top {} bottom {}".format(crop_top, height - expected_height - crop_top))
        return image
    if height < expected_height:
        top_border = int((expected_height - height) / 2)
        bottom_border = expected_height - height - top_border
        image=cv2.copyMakeBorder(image, top=top_border, bottom=bottom_border, left=0, right=0, borderType= cv2.BORDER_CONSTANT)
        #print("  added border: top {}, bottom {}".format(top_border, bottom_border))
        return image


def calcError(im1, im2):
    norm = cv2.norm(im1, im2)
    hist1 = cv2.calcHist([im1],[0],None,[256],[0,256])
    hist2 = cv2.calcHist([im2],[0],None,[256],[0,256])
    error = 1 - cv2.compareHist(hist1,hist2,cv2.HISTCMP_CORREL)
    error *= 100
    return error, norm

def compareWithMulti(n):
    filename_multi_n = filename_multi.format(n)
    multi = cv2.imread(filename_multi_n, 0)
    multi = fixImage(multi)

    single = cv2.imread(filename_single, 0)
    single = fixImage(single)

    error, norm = calcError(single, multi)
    #print("Multi {}: error {:.2f}% norm: {:.0f}".format(n, error, norm))
    print("({}, {:.2f})".format(n, error))
    #print("({}, {:.2f})".format(n, norm))


def main():
    n = 0
    while n < 20:
        n += 1
        compareWithMulti(n)

    print("Completed!")

if __name__ == "__main__":
    main()