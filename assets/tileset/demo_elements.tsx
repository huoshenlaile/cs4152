<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="ragdoll_elements" tilewidth="2581" tileheight="1080" tilecount="15" columns="0" objectalignment="bottomleft">
 <editorsettings>
  <export target="ragdoll_elements..tsx" format="tsx"/>
 </editorsettings>
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="7" type="exit">
  <properties>
   <property name="properties" type="class" propertytype="ExitProperties">
    <properties>
     <property name="obstacle" type="class" propertytype="Obstacle">
      <properties>
       <property name="bodytype" type="int" propertytype="bodytype" value="0"/>
       <property name="damping" type="float" value="0"/>
       <property name="debugcolor" propertytype="debugcolor" value="yellow"/>
       <property name="debugopacity" type="float" value="192"/>
       <property name="density" type="float" value="0"/>
       <property name="friction" type="float" value="0"/>
       <property name="restitution" type="float" value="0"/>
       <property name="rotation" type="bool" value="false"/>
       <property name="texture" value="goal"/>
      </properties>
     </property>
    </properties>
   </property>
  </properties>
  <image width="96" height="96" source="goaldoor.png"/>
 </tile>
 <tile id="9" type="wall">
  <properties>
   <property name="properties" type="class" propertytype="WallProperties">
    <properties>
     <property name="obstacle" type="class" propertytype="Obstacle">
      <properties>
       <property name="bodytype" type="int" propertytype="bodytype" value="1"/>
       <property name="damping" type="float" value="0"/>
       <property name="debugcolor" propertytype="debugcolor" value=""/>
       <property name="debugopacity" type="float" value="192"/>
       <property name="density" type="float" value="0"/>
       <property name="friction" type="float" value="0"/>
       <property name="restitution" type="float" value="0"/>
       <property name="rotation" type="bool" value="false"/>
       <property name="texture" value="earth"/>
      </properties>
     </property>
    </properties>
   </property>
  </properties>
  <image width="214" height="100" source="../textures/popleft.png"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="9.46814" y="10.0604" width="172.615" height="84.5282"/>
  </objectgroup>
 </tile>
 <tile id="10">
  <image width="100" height="100" source="../textures/goalpop.png"/>
 </tile>
 <tile id="11">
  <image width="214" height="100" source="../textures/popright.png"/>
 </tile>
 <tile id="12">
  <image width="1500" height="700" source="../textures/popart_platformbubble_left.png"/>
 </tile>
 <tile id="13">
  <image width="1500" height="700" source="../textures/popart_platformbubble_right.png"/>
 </tile>
 <tile id="14">
  <image width="2501" height="301" source="../textures/popart_platformLine.png"/>
 </tile>
 <tile id="15">
  <image width="700" height="700" source="../textures/popart_goal.png"/>
 </tile>
 <tile id="16">
  <image width="2340" height="1080" source="../textures/popart_background.png"/>
 </tile>
 <tile id="22">
  <image width="2581" height="580" source="../textures/alpharelease_background.png"/>
 </tile>
 <tile id="24">
  <image width="232" height="476" source="../textures/alpharelease_soy_capon.png"/>
 </tile>
 <tile id="25">
  <image width="250" height="463" source="../textures/alpharelease_mustard.png"/>
 </tile>
 <tile id="26">
  <image width="193" height="443" source="../textures/alpharelease_mayo_capon.png"/>
 </tile>
 <tile id="27">
  <image width="193" height="500" source="../textures/alpharelease_ketchup_capon.png"/>
 </tile>
 <tile id="28">
  <image width="193" height="500" source="../textures/alpharelease_ketchup_capoff.png"/>
 </tile>
</tileset>
