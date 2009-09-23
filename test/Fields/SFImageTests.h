/*
 * Copyright 2009 Nathan Matthews <lowentropy@gmail.com>
 *
 * This file is part of SimpleX3D.
 * 
 * SimpleX3D is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * SimpleX3D is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with SimpleX3D.  If not, see <http://www.gnu.org/licenses/>.
 */

using ::testing::NotNull;

TEST(SFImage, EmptyImageHasNullBytes) {
	SFImage image(0,0,0);
	EXPECT_EQ(NULL, image.array()) << "Empty image has bytes.";
}

TEST(SFImage, NonEmptyImageShouldHaveBytes) {
	SFImage image(1,1,1);
	EXPECT_THAT(image.array(), NotNull());
}

TEST(SFImage, ShouldThrowOnNegativeDims) {
    SFImage* image;
	ASSERT_ANY_THROW(image = new SFImage(-1,1,1));
	ASSERT_ANY_THROW(image = new SFImage(1,-1,1));
}

TEST(SFImage, ShouldThrowWhenAccessingOutOfBoundsCoord) {
    SFImage image(1,1,1);
	EXPECT_ANY_THROW(image.getPixel(1,1));
	EXPECT_ANY_THROW(image.setPixel(1,1, 0));
	EXPECT_ANY_THROW(image.getColorRGBA(1,1));
	EXPECT_ANY_THROW(image.setColorRGBA(1,1, SFColorRGBA()));
	EXPECT_ANY_THROW(image.getColor(1,1));
	EXPECT_ANY_THROW(image.setColor(1,1, SFColor()));
}
 
TEST(SFImage, ShouldAllowOnlyCorrectComponentsValues) {
	SFImage* image;
	ASSERT_NO_THROW(image = new SFImage(0,0,0)); // empty, default
	ASSERT_NO_THROW(image = new SFImage(1,1,1));
	ASSERT_NO_THROW(image = new SFImage(1,1,2));
	ASSERT_NO_THROW(image = new SFImage(1,1,3));
	ASSERT_NO_THROW(image = new SFImage(1,1,4));
	ASSERT_ANY_THROW(image = new SFImage(1,1,0));
	ASSERT_ANY_THROW(image = new SFImage(1,1,5));
}

TEST(SFImage, CopyConstructorShouldCopyBytes) {
	SFImage orig(2,2,2);
	for (int i = 0; i < 8; i++)
		orig.array()[i] = rand();
	ASSERT_EQ(8, orig.getSize());
	SFImage copy(orig);
	ASSERT_NE(orig.array(), copy.array());
	for (int i = 0; i < 8; i++)
		ASSERT_EQ(orig.array()[i], copy.array()[i]);
}
