
img = imread('/home/21104511/Documents/master/oim/tp2/image');
taille = size(img);
seuilBleu = 120;
seuilRouge = 113;
 imgf = uint8(zeros(taille));
 for x=1:taille(1)
     for y=1:taille(2)
          if img(x,y,3) > seuilBleu
             if img(x,y,1) < seuilRouge
                 imgf(x,y,1) = img(x,y,3);
                 imgf(x,y,2) = img(x,y,1);
                 imgf(x,y,3) = img(x,y,2);
             end
          end
     end
 end

 for x=1:taille(1)
     for y=1:taille(2)
          if imgf(x,y,3) ~= 0
            img(x,y,1) = imgf(x,y,1);
            img(x,y,2) = img(x,y,2);
            img(x,y,3) = img(x,y,3);
          end
     end
 end

 