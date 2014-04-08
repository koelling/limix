// Copyright(c) 2014, The LIMIX developers (Christoph Lippert, Paolo Francesco Casale, Oliver Stegle)
// All rights reserved.
//
// LIMIX is provided under a 2-clause BSD license.
// See license.txt for the complete license.

#if !defined( TsvLexer_h )
#define TsvLexer_h
/*
 *******************************************************************
 *
 *    Copyright (c) Microsoft. All rights reserved.
 *    This code is licensed under the Apache License, Version 2.0.
 *    THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
 *    ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
 *    IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
 *    PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
 *
 ******************************************************************
 */

/*
 * TsvLexer - {Tab Separated Values Scanner/Lexer Routines}
 *
 *         File Name:   TsvLexer.h
 *           Version:   1.00
 *            Author:   
 *     Creation Date:   18 Nov 2010
 *     Revision Date:   18 Nov 2010
 *    Module Purpose:   This file declares the TsvScanner routines 
 *                      for FastLmmC
 *    Change History:   
 *
 * Test Files: 
 */

/*
 * Get our includes
 */
#include "Cplink.h"
/*
 * 'Publish' our defines
 */

/*
 * 'Publish' our class declarations / function prototypes
 */
class CTsvLexer : CLexer
   {
public:
   CTsvLexer( std::string filename );
   ~CTsvLexer();
   unsigned NextToken( CToken& tok );
   };

/*
 * 'Publish' the globals we define
 */

#endif   // TsvLexer_h
