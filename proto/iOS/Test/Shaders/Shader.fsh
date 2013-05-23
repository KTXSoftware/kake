//
//  Shader.fsh
//  Test
//
//  Created by Robert Konrad on 23.07.11.
//  Copyright 2011 KonTechs Limited. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
