from uwimg import *
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-box7")

from uwimg import *
im = load_image("data/dog.jpg")
f = make_box_filter(7)
blur = convolve_image(im, f, 1)
thumb = nn_resize(blur, blur.w//7, blur.h//7)
save_image(thumb, "dogthumb")

from uwimg import *
im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
blur = convolve_image(im, f, 1)
save_image(blur, "dog-gauss2")

from uwimg import *
im = load_image("data/dog.jpg")
f = make_gaussian_filter(2)
lfreq = convolve_image(im, f, 1)
hfreq = im - lfreq
reconstruct = lfreq + hfreq
save_image(lfreq, "low-frequency")
save_image(hfreq, "high-frequency")
save_image(reconstruct, "reconstruct")

# Ronbledore time! :D
from uwimg import *
im_a = load_image("data/dumbledore.png")
im_b = load_image("data/ron.png")
f_a = make_gaussian_filter(2)
lfreq_a = convolve_image(im_a, f_a, 1)
f_b = make_gaussian_filter(2)
lfreq_b = convolve_image(im_b, f_b, 1)
hfreq_b = im_b - lfreq_b
combined = lfreq_a + hfreq_b
save_image(combined, "ronbledore")


