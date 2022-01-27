local t =  {
                                        (function(a1, a2, a3, a4, a5, a6)
                                            return {
                                                [function()
                                                    aaa, bbb, ccc = 1, 2, 3

                                                    return aa, bb, cc, dd, ee
                                                end] = {
                                                    gggg
                                                }
                                            }
                                        end)(1, 2, 3, 4, 5, 6)
                                    }
