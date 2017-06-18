<BODY>

<DIV id="id_1">
<H1> XelSets Image Segmentation</H1>
</DIV>
<DIV id="id_2_1">
<P class="p14 ft6"><H2> Introduction and Pre-processing </H2></P>
<P class="p10 ft6">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;As an extension for "Broader framework for lensometry". Let's make lens fake data by scaling the values of pixels at the center region and considering different shapes as shown.
</P>
</DIV>

| Circle | Oval |
| ------------- | ------------- |
| ![image](https://cloud.githubusercontent.com/assets/11435669/21528469/c343ee2c-cd00-11e6-900b-c0bd3b49d711.png)  | ![image](https://cloud.githubusercontent.com/assets/11435669/21528481/d512bf02-cd00-11e6-83ac-41b16987c758.png)  | 
|  Bifocal Ellipse  |  Bifocal Line |
| ![image](https://cloud.githubusercontent.com/assets/11435669/21528516/19ad4678-cd01-11e6-8aaf-d78b719fc06e.png)  | ![image](https://cloud.githubusercontent.com/assets/11435669/21528526/289c46fc-cd01-11e6-82c2-5a58894b56a5.png) | 


<DIV id="id_2_2">
<P class="p12 ft6">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: "Circle" is scaled inside circle, "Oval" is scaled inside ellipse,
"Bifocal Ellipse" is scaled in opposite directions in each segments of ellipse, lastly "Bifocal Line" is similar to
"Bifocal Ellipse",expect ellipse is segmented using a line (not ellipse) . 
</P>
</DIV>


<DIV id="id_2_3">
<P class="p14 ft6"><H2>  Segmentation of lens Image </H2></P>
<P class="p14 ft6"><H5>  Segmentation using segmentation matrix: </H5></P>
<P class="p15 ft6">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;We segment (fake data) lensed mire
images by running weighted matrix all over the image and we get the following segments:
</P>
</DIV>



| Circle | Oval |
| ------------- | ------------- |
| ![image](https://cloud.githubusercontent.com/assets/11435669/21528623/e138a9f8-cd01-11e6-95b3-9312ba96c3ee.png)  | ![image](https://cloud.githubusercontent.com/assets/11435669/21528635/ef16657e-cd01-11e6-861e-9f8527a540c3.png)  | 
|  Bifocal Ellipse  |  Bifocal Line |
| ![image](https://cloud.githubusercontent.com/assets/11435669/21528649/058a1274-cd02-11e6-9c03-8fdede02dc5e.png)  | ![image](https://cloud.githubusercontent.com/assets/11435669/21528651/0f3dc7b6-cd02-11e6-8827-efeda7fb5007.png) | 



<DIV id="id_1_2">
<P class="p22 ft10"><SPAN class="ft10"><H5> Algorithm for the above logic is as follows: </H5></P>
<P class="p23 ft9">
	<ul>
	  <li>Take a matrix size. For our image of 3000*3000, we have taken mask of 25*25. </li>
	  <li>In this 25*25 range find maximum and minimum pixel value. </li>
	  <li>If the difference of the maximum and minimum values exceeds some threshold values then enter the loop. </li>
	  <li>Inside the loop we make 25*25 portion of our image turn to black (0 pixel value). </li>
	  <li>Run the 25*25 matrix mask all over mire image and repeat Steps 1 to 4 for complete mire image. </li>
	</ul>
</P>
</DIV>


<DIV id="id_2_3">
<P class="p14 ft6"><H5>  Segmentation using snakes: </H5></P>
<P class="p15 ft6">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; From previous case the last two images of (for bifocals) the segmentation is not complete and hence we further try to segment these using snakes. The image shows that snakes don’t suit for our application as it has energy cost for bending and we are trying to make sudden bends.
</P>
</DIV>
<p><a href="https://cloud.githubusercontent.com/assets/11435669/21528770/ebd1fba2-cd02-11e6-97d3-1ca412ffd6e9.png" target="_blank"><img src="https://cloud.githubusercontent.com/assets/11435669/21528770/ebd1fba2-cd02-11e6-97d3-1ca412ffd6e9.png" alt="image" style="max-width:100%;"></a></p>

<DIV id="id_2_3">
<P class="p14 ft6"><H5>  Segmentation using XelSets: </H5></P>
<P class="p15 ft6">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;  Snakes have energy cost for bending, hence they perform great with smooth loops but they cannot make sudden bends like a square or edge surface. So, we alter this algorithm. Hence for our application and other medical application like segmenting veins and blood vessels we consider xelset to play upper hand in segmentation. Hexagons are drawn using opengl tool and the pitchx, pitchy, minor radius and major radius are as shown in the figure. Due to the obvious shape of hexagon there are more number of columns than rows and hence our image is stretched by this proportion. i.e. (2/(√ 3)).
</P>
</DIV>
![image](https://cloud.githubusercontent.com/assets/11435669/21528849/5be8f530-cd03-11e6-9a23-8fac64b33bc4.png)


<DIV id="id_2_3">
<P class="p15 ft6">The points are moved by two kinds of force: intrinsic to the xelset, and extrinsic
	<ul>
	  <li>If movement puts two points into the same xel, they merge.</li>
	  <li>If the points ‘p’ and ‘q’ cease to be neighbours, a new point appears in the xel between them, on the line PQ </li>
	</ul>
</P>
</DIV>

<DIV id="id_2_3">
<P class="p15 ft6">Each point is subject to two kinetic rules:
	<ul>
	  <li>Extrinsic forces correspond to: take the image and get black and white version of this image. Complementing the image will make black region white. Now if we apply Gaussian blur with blur count representing the size of the mask, the white region is bright region. When we first load the image there are certain criteria on which we get threshold by using logic of bottoms and these points move towards brighter region giving our segmentation.</li>
	  <li>Intrinsic forces: applying just extrinsic forces doesn’t guarantee the straightness of the curve hence we take tangents at each point and see to it that neighbours are almost of same direction of the tangent at that point.  </li>
	</ul>
	Now running the xelset code on primitive segments give us:
</P>
</DIV>


| Circle | Oval |
| ------------- | ------------- |
| ![image](https://cloud.githubusercontent.com/assets/11435669/21528984/796774be-cd04-11e6-988d-91e1a3d43351.png)  | ![image](https://cloud.githubusercontent.com/assets/11435669/21528995/905aeb42-cd04-11e6-9f0e-658c36d45acc.png)  | 
|  Bifocal Ellipse  |  Bifocal Line |
| ![image](https://cloud.githubusercontent.com/assets/11435669/21529010/a3889a0c-cd04-11e6-883b-d396fec33cc7.png)  | ![image](https://cloud.githubusercontent.com/assets/11435669/21529024/b5c0f476-cd04-11e6-805b-440d9cf7fb12.png) | 


<DIV id="id_1_2">
<P class="p80 ft10"><SPAN class="ft10"><H2> References </H2></P>
<P class="p136 ft59"><SPAN class="ft29">[1]&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</SPAN>“XelSets segmentation” Dr. Tim Poston. </P>
</DIV>

</BODY>
</HTML>
