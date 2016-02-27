img = double(imread('/home/21104511/Documents/master/oim/tp2/image'));
taille = size(img);

 imgg = uint8(zeros(taille(1),taille(2)));
 for x=1:taille(1)
     for y=1:taille(2)
         var = uint8((img(x,y,1)+img(x,y,2)+img(x,y,3)) / 3);
         if var > 128
            imgg(x,y) = 255;
         else
             imgg(x,y) = 0;
         end 
     end
 end

 
 